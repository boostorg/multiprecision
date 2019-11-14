//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MODULAR_ADAPTOR_HPP
#define BOOST_MULTIPRECISION_MODULAR_ADAPTOR_HPP

#include <boost/cstdint.hpp>
#include <boost/multiprecision/detail/digits.hpp>
#include <boost/functional/hash_fwd.hpp>
#include <boost/multiprecision/number.hpp>

#include <boost/multiprecision/modular/modular_params.hpp>
#include <boost/container/small_vector.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

namespace boost {
namespace multiprecision {
namespace backends {

template <typename Backend>
class modular_adaptor
{
 protected:
   Backend                 m_base;
   modular_params<Backend> m_mod;

 public:
   inline Backend& base_data() { return m_base; }

   inline Backend const& base_data() const { return m_base; }

   inline modular_params<Backend>& mod_data() { return m_mod; }

   inline const modular_params<Backend>& mod_data() const { return m_mod; }

   typedef typename Backend::signed_types   signed_types;
   typedef typename Backend::unsigned_types unsigned_types;

   modular_adaptor() {}

   modular_adaptor(const modular_adaptor& o) : m_base(o.base_data()), m_mod(o.mod_data())
   {
   }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

   modular_adaptor(modular_adaptor&& o)
       : m_base(std::move(o.base_data())), m_mod(std::move(o.mod_data()))
   {}

#endif
   modular_adaptor(const Backend& val, const modular_params<Backend>& mod) : m_mod(mod), m_base(mod.create_internal_representation(val))
   {

   }

   modular_adaptor(const Backend& val, const Backend& mod)
       : m_base(val), m_mod(mod)
   {
   }

   modular_adaptor(Backend& val, Backend& mod)
       : m_base(val), m_mod(mod) {}

   modular_adaptor(const Backend& val) : m_base(val), m_mod(typename mpl::front<unsigned_types>::type(0u))
   {
   }

   modular_adaptor(const modular_params<Backend>& mod) : m_base(typename mpl::front<unsigned_types>::type(0u)), m_mod(mod)
   {
   }

   modular_adaptor& operator=(const modular_adaptor& o)
   {
      m_base = o.base_data();
      m_mod = o.mod_data();
      return *this;
   }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

   modular_adaptor& operator=(modular_adaptor&& o)

       BOOST_NOEXCEPT
   {
      m_base = std::move(o.base_data());
      m_mod  = std::move(o.mod_data());
      return *this;
   }
#endif

   modular_adaptor& operator=(const char* s)
   {
      typedef typename mpl::front<unsigned_types>::type ui_type;
      ui_type                                           zero = 0u;

      using default_ops::eval_fpclassify;

      if (s && (*s == '('))
      {
         std::string part;
         const char* p = ++s;
         while (*p && (*p != ',') && (*p != ')'))
            ++p;
         part.assign(s, p);
         if (!part.empty())
            m_base() = part.c_str();
         else
            m_base() = zero;
         s = p;
         if (*p && (*p != ')'))
         {
            ++p;
            while (*p && (*p != ')'))
               ++p;
            part.assign(s + 1, p);
         }
         else
            part.erase();
         if (!part.empty())
            m_mod() = part.c_str();
         else
            m_mod() = zero;
      }
      else
      {
         base_data() = s;
         m_mod()     = zero;
      }
      return *this;
   }

   int compare(const modular_adaptor& o) const
   {
      // They are either equal or not:<
      if (m_mod.compare(o.mod_data()) != 0)
      {
         BOOST_THROW_EXCEPTION(std::runtime_error("Could not compare modular number with different mod."));
      }
      Backend tmp1, tmp2;
      mod_data().adjust_regular(tmp1, base_data());
      mod_data().adjust_regular(tmp2, o.base_data());
      return tmp1.compare(tmp2);
   }

   template <class T>
   int compare(const T& val) const
   {
      using default_ops::eval_lt;
      if (!eval_lt(m_mod, val))
      {
         BOOST_THROW_EXCEPTION(std::runtime_error("Could not compare modular number with mod bigger than compared number."));
      }
      Backend tmp;
      mod_data().adjust_regular(tmp, base_data());
      return tmp.compare(val);
   }

   inline void swap(modular_adaptor& o)
   {
      base_data().swap(o.base_data());
      std::swap(mod_data(), o.mod_data());
   }

   inline std::string str(std::streamsize dig, std::ios_base::fmtflags f) const
   {
      Backend tmp;
      mod_data().adjust_regular(tmp, base_data());
      return tmp.str(dig, f);
   }

   inline void negate()
   {
      base_data().negate();
      eval_add(base_data(), mod_data().get_mod().backend());
   }

   template <typename BackendT, expression_template_option ExpressionTemplates>
   operator number<BackendT, ExpressionTemplates>()
   {
      return base_data();
   };
};

template <class Result, class Backend>
inline void eval_convert_to(Result* result, const modular_adaptor<Backend>& val)
{
   using default_ops::eval_convert_to;
   eval_convert_to(result, val.base_data());
}

template <class Backend, class T>
inline typename enable_if<is_arithmetic<T>, bool>

    ::type eval_eq(const modular_adaptor<Backend>& a,
                   const T&                        b)

        BOOST_NOEXCEPT
{
   return a.compare(b) == 0;
}

template <class Backend>
inline void eval_redc(Backend& result, const modular_params<Backend>& mod)
{
   mod.reduce(result);
}

template <class Backend>
inline void eval_add(modular_adaptor<Backend>&       result,
                     const modular_adaptor<Backend>& o)
{
   BOOST_ASSERT(result.mod_data().get_mod() == o.mod_data().get_mod());
   using default_ops::eval_gt;
   eval_add(result.base_data(), o.base_data());
   if (eval_gt(result.base_data(), result.mod_data().get_mod().backend()))
   {
      eval_subtract(result.base_data(), result.mod_data().get_mod().backend());
   }
}

template <class Backend>
inline void eval_subtract(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& o)
{
   BOOST_ASSERT(result.mod_data().get_mod() == o.mod_data().get_mod());
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
   using default_ops::eval_lt;
   eval_subtract(result.base_data(), o.base_data());
   if (eval_lt(result.base_data(), ui_type(0u)))
   {
      eval_add(result.base_data(), result.mod_data().get_mod().backend());
   }
}

template <class Backend>
inline void eval_multiply(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& o)
{
   BOOST_ASSERT(result.mod_data().get_mod() == o.mod_data().get_mod());
   eval_multiply(result.base_data(), o.base_data());
   eval_redc(result.base_data(), result.mod_data());
}

template <class Backend>
inline void eval_divide(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& o)
{
   BOOST_ASSERT(result.mod_data().get_mod() == o.mod_data().get_mod());
   Backend tmp1, tmp2;
   result.mod_data().adjust_regular(tmp1, result.base_data());
   result.mod_data().adjust_regular(tmp2, o.base_data());
   eval_divide(tmp1, tmp2);
   result.base_data() = tmp1;
   result.mod_data().adjust_modular(result.base_data());
   result.mod_data().adjust_regular(tmp2, result.base_data());
}

template <class Backend>
inline void eval_modulus(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& o)
{
   BOOST_ASSERT(result.mod_data().get_mod() == o.mod_data().get_mod());
   Backend tmp1, tmp2;
   result.mod_data().adjust_regular(tmp1, result.base_data());
   result.mod_data().adjust_regular(tmp2, o.base_data());
   eval_modulus(tmp1, tmp2);
   result.base_data() = tmp1;
   result.mod_data().adjust_modular(result.base_data());
   result.mod_data().adjust_regular(tmp2, result.base_data());
}

template <class Backend>
inline bool eval_is_zero(const modular_adaptor<Backend>& val)

    BOOST_NOEXCEPT
{
   using default_ops::eval_is_zero;
   return eval_is_zero(val.base_data());
}

template <class Backend>
inline int eval_get_sign(const modular_adaptor<Backend>&)
{
   return 1;
}

template <class Result, class Backend>
inline typename disable_if_c<boost::is_complex<Result>::value>::type
eval_convert_to(Result* result, const modular_adaptor<Backend>& val)
{
   using default_ops::eval_convert_to;
   eval_convert_to(result, val.base_data());
}

template <class Backend, class T, class V>
inline void assign_components(modular_adaptor<Backend>& result,
                              const T& a, const V& b)
{
   result.base_data() = a;
   result.mod_data()  = b;
   result.mod_data().adjust_modular(result.base_data());
}

template <class Backend>
inline void eval_sqrt(modular_adaptor<Backend>&       result,
                      const modular_adaptor<Backend>& val)
{
   eval_sqrt(result.base_data(), val.base_data());
}

template <class Backend>
inline void eval_abs(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& val)
{
   result = val;
}

size_t window_bits(size_t exp_bits)
{
   BOOST_STATIC_CONSTEXPR size_t wsize_count           = 6;
   BOOST_STATIC_CONSTEXPR size_t wsize[wsize_count][2] = {
       {1434, 7},
       {539, 6},
       {197, 4},
       {70, 3},
       {17, 2},
       {0, 0}};

   size_t window_bits = 1;

   size_t j = wsize_count - 1;
   while ((wsize[j][0] > exp_bits) && (j >= 0))
   {
      --j;
   }
   window_bits += wsize[j][1];

   return window_bits;
};

template <class Backend>
inline void find_modular_pow(modular_adaptor<Backend>&       result,
                             const modular_adaptor<Backend>& b,
                             Backend&                        exp)
{
   //   using default_ops::eval_bit_test;
   //   using default_ops::eval_gt;
   //   typedef typename boost::multiprecision::detail::canonical<unsigned char, Backend>::type ui_type;
   //   typedef number<modular_adaptor<Backend> > modular_type;
   //   modular_params<Backend>  mod = b.mod_data();
   //   modular_type x(1, mod);
   //   modular_type y = b;
   //   while (eval_gt(exp, ui_type(0u)))
   //   {
   //      if (eval_bit_test(exp, ui_type(0u)))
   //      {
   //         x = x * y;
   //      }
   //      y = y * y;
   //      eval_right_shift(exp, ui_type(1u));
   //   }
   //   result = x.backend();

   using default_ops::eval_bit_set;
   using default_ops::eval_decrement;
   using default_ops::eval_multiply;

   typedef number<modular_adaptor<Backend> > modular_type;
   modular_params<Backend>                   mod = b.mod_data();
   size_t                                    m_window_bits;
   unsigned long                             cur_exp_index;
   size_t                                    exp_bits = eval_msb(exp);
   m_window_bits                                      = window_bits(exp_bits + 1);
   modular_type* m_g = (modular_type*)std::malloc((1U << m_window_bits) * sizeof(modular_type));
   modular_type              x(1, mod);

   Backend nibble = exp;
   Backend mask;
   eval_bit_set(mask, m_window_bits);
   eval_decrement(mask);
   m_g[0] = x;
   m_g[1] = b;
   for (size_t i = 2; i < (1U << m_window_bits); i++)
   {
      eval_multiply(m_g[i].backend(), m_g[i - 1].backend(), b);
   }
   size_t exp_nibbles = (exp_bits + 1 + m_window_bits - 1) / m_window_bits;
   std::vector<size_t> exp_index;

   for (size_t i = 0; i < exp_nibbles; ++i)
   {
      Backend tmp = nibble;
      eval_bitwise_and(tmp, mask);
      eval_convert_to(&cur_exp_index, tmp);
      eval_right_shift(nibble, m_window_bits);
      exp_index.push_back(cur_exp_index);
   }

   x = x * m_g[exp_index[exp_nibbles - 1]];
   for (size_t i = exp_nibbles - 1; i > 0; --i)
   {

      for (size_t j = 0; j != m_window_bits; ++j)
      {
         x = x * x;
      }

      x = x * m_g[exp_index[i - 1]];
   }
   result = x.backend();
   std::free(m_g);
}

template <class Backend>
inline void eval_pow(modular_adaptor<Backend>&       result,
                     const modular_adaptor<Backend>& b,
                     const modular_adaptor<Backend>& e)
{
   Backend exp;
   e.mod_data().adjust_regular(exp, e.base_data());
   find_modular_pow(result, b, exp);
}

template <class Backend>
inline void eval_pow(modular_adaptor<Backend>&       result,
                     const modular_adaptor<Backend>& b,
                     const Backend&                  e)
{
   Backend exp = e;
   find_modular_pow(result, b, exp);
}

template <class Backend, class UI>
inline void eval_left_shift(modular_adaptor<Backend>& t, UI i)
{
   using default_ops::eval_left_shift;
   Backend tmp;
   t.mod_data().adjust_regular(tmp, t.base_data());
   eval_left_shift(tmp);
   t.base_data() = tmp;
   t.mod_data().adjust_modular(t.base_data());
}
template <class Backend, class UI>
inline void eval_right_shift(modular_adaptor<Backend>& t, UI i)
{
   using default_ops::eval_right_shift;
   Backend tmp;
   t.mod_data().adjust_regular(tmp, t.base_data());
   eval_right_shift(tmp);
   t.base_data() = tmp;
   t.mod_data().adjust_modular(t.base_data());
}
template <class Backend, class UI>
inline void eval_left_shift(modular_adaptor<Backend>& t, const modular_adaptor<Backend>& v, UI i)
{
   using default_ops::eval_left_shift;
   Backend tmp1, tmp2;
   t.mod_data().adjust_regular(tmp1, t.base_data());
   t.mod_data().adjust_regular(tmp2, v.base_data());
   eval_left_shift(tmp1, tmp2, static_cast<unsigned long>(i));
   t.base_data() = tmp1;
   t.mod_data().adjust_modular(t.base_data());
}
template <class Backend, class UI>
inline void eval_right_shift(modular_adaptor<Backend>& t, const modular_adaptor<Backend>& v, UI i)
{
   using default_ops::eval_right_shift;
   Backend tmp1, tmp2;
   t.mod_data().adjust_regular(tmp1, t.base_data());
   t.mod_data().adjust_regular(tmp2, v.base_data());
   eval_right_shift(tmp1, tmp2, static_cast<unsigned long>(i));
   t.base_data() = tmp1;
   t.mod_data().adjust_modular(t.base_data());
}

template <class Backend>
inline void eval_bitwise_and(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& v)
{
   using default_ops::eval_bitwise_and;
   Backend tmp1, tmp2;
   result.mod_data().adjust_regular(tmp1, result.base_data());
   result.mod_data().adjust_regular(tmp2, v.base_data());
   eval_bitwise_and(tmp1, tmp1, tmp2);
   result.base_data() = tmp1;
   result.mod_data().adjust_modular(result.base_data());
}

template <class Backend>
inline void eval_bitwise_or(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& v)
{
   using default_ops::eval_bitwise_or;
   Backend tmp1, tmp2;
   result.mod_data().adjust_regular(tmp1, result.base_data());
   result.mod_data().adjust_regular(tmp2, v.base_data());
   eval_bitwise_or(tmp1, tmp1, tmp2);
   result.base_data() = tmp1;
   result.mod_data().adjust_modular(result.base_data());
}

template <class Backend>
inline void eval_bitwise_xor(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& v)
{
   using default_ops::eval_bitwise_xor;
   Backend tmp1, tmp2;
   result.mod_data().adjust_regular(tmp1, result.base_data());
   result.mod_data().adjust_regular(tmp2, v.base_data());
   eval_bitwise_xor(tmp1, tmp1, tmp2);
   result.base_data() = tmp1;
   result.mod_data().adjust_modular(result.base_data());
}

} // namespace backends

using boost::multiprecision::backends::modular_adaptor;

template <class Backend>
struct number_category<modular_adaptor<Backend> > : public boost::mpl::int_<boost::multiprecision::number_kind_modular>
{};

template <class Backend, expression_template_option ExpressionTemplates>
struct component_type<number<modular_adaptor<Backend>, ExpressionTemplates> >
{
   typedef number<Backend, ExpressionTemplates> type;
};

}

} // namespace boost::multiprecision

#endif
