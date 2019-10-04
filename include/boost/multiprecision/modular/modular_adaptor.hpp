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
#include <boost/multiprecision/modular/mask_bits.hpp>

#include <algorithm>
#include <cmath>

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
   Backend& base_data() { return m_base; }

   Backend const& base_data() const { return m_base; }

   modular_params<Backend>& mod_data() { return m_mod; }

   const modular_params<Backend>& mod_data() const { return m_mod; }

   typedef typename Backend::signed_types   signed_types;
   typedef typename Backend::unsigned_types unsigned_types;

   modular_adaptor() {}

   modular_adaptor(const modular_adaptor& o)
   {
      m_base = o.base_data();
      m_mod  = o.mod_data();
   }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

   modular_adaptor(modular_adaptor&& o)
       : m_base(std::move(o.base_data())), m_mod(std::move(o.mod_data()))
   {}

#endif
   modular_adaptor(const Backend& val, const montgomery_params<Backend>& mod)
   {
      m_mod  = mod;
      m_base = mod.create_internal_representation(val);
   }

   modular_adaptor(const Backend& val, const Backend& mod)
       : m_base(val), m_mod(mod)
   {
   }

   modular_adaptor(Backend& val, Backend& mod)
       : m_base(val), m_mod(mod) {}

   modular_adaptor(const Backend& val)
   {
      typedef typename mpl::front<unsigned_types>::type ui_type;
      m_base = val;
      m_mod  = ui_type(0u);
   }

   modular_adaptor(const modular_params<Backend>& mod)
   {
      typedef typename mpl::front<unsigned_types>::type ui_type;
      m_base = ui_type(0u);
      m_mod  = mod;
   }

   modular_adaptor& operator=(const modular_adaptor& o)
   {
      m_base = o.base_data();
      m_mod  = o.mod_data();
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
         if (part.size())
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
         if (part.size())
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
      // They are either equal or not:
      return (m_base.compare(o.base_data()) == 0) &&
                     (m_mod.compare(o.mod_data()) == 0)
                 ? 0
                 : 1;
   }

   template <class T>
   int compare(const T& val) const
   {
      using default_ops::eval_lt;
      return (m_base.compare(val) == 0) && eval_lt(m_mod, val) ? 0 : 1;
   }

   void swap(modular_adaptor& o)
   {
      base_data().swap(o.base_data());
      mod_data().swap(o.mod_data());
   }

   std::string str(std::streamsize dig, std::ios_base::fmtflags f) const
   {
      Backend tmp;
      mod_data().adjust_regular(tmp, base_data());
      return tmp.str(dig, f);
   }
};

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
   eval_add(result.base_data(), o.base_data());
}

template <class Backend>
inline void eval_subtract(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& o)
{
   if (eval_lt(result.base_data(), o.base_data()))
   {
      BOOST_THROW_EXCEPTION(std::range_error("Modular less than zero"));
   }
   eval_subtract(result.base_data(), o.base_data());
}

template <class Backend>
inline void eval_multiply(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& o)
{
   eval_multiply(result.base_data(), o.base_data());
   eval_redc(result.base_data(), result.mod_data());
}

template <class Backend>
inline void eval_divide(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& o)
{
   eval_divide(result.base_data(), o.base_data());
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

   size_t j = 0;
   while (wsize[j][0] < exp_bits && j < wsize_count)
   {
      ++j;
   }
   window_bits += wsize[j][1];

   return window_bits;
};


template <class Backend>
inline void find_modular_pow(modular_adaptor<Backend>& result,
                             const modular_adaptor<Backend>& b,
                             Backend& exp)
{
   typedef number<modular_adaptor<Backend> > modular_type;

   modular_params<Backend>   mod = b.mod_data();
   size_t                    m_window_bits;
   std::vector<modular_type> m_g;
   uint32_t                  exp_index;
   m_window_bits = window_bits(eval_msb(exp) + 1);
   modular_type x(1, mod);
   m_g.push_back(x);
   m_g.push_back(b);
   for (size_t i = 2; i != 1 << m_window_bits; ++i)
   {
      m_g.push_back(m_g[i - 1] * m_g[1]);
   }

   const size_t exp_nibbles = (eval_msb(exp) + 1 + m_window_bits - 1) / m_window_bits;
   for (size_t i = exp_nibbles; i > 0; --i)
   {

      for (size_t j = 0; j != m_window_bits; ++j)
      {
         x = x * x;
      }

      Backend nibble = exp;
      eval_right_shift(nibble, m_window_bits * (i - 1));
      eval_mask_bits(nibble, m_window_bits);
      eval_convert_to(&exp_index, nibble);
      x = x * m_g[exp_index];
   }
   result = x.backend();
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
                     const Backend& e)
{
   Backend exp = e;
   find_modular_pow(result, b, exp);
}

template <class Backend, class UI>
inline void eval_left_shift(modular_adaptor<Backend>& t, UI i)
{
   eval_left_shift(t.base_data());
}
template <class Backend, class UI>
inline void eval_right_shift(modular_adaptor<Backend>& t, UI i)
{
   eval_right_shift(t.base_data());
   eval_redc(t.base_data(), t.mod_data());
}
template <class Backend, class UI>
inline void eval_left_shift(modular_adaptor<Backend>& t, const modular_adaptor<Backend>& v, UI i)
{
   eval_left_shift(t.base_data(), v.base_data(), static_cast<unsigned long>(i));
}
template <class Backend, class UI>
inline void eval_right_shift(modular_adaptor<Backend>& t, const modular_adaptor<Backend>& v, UI i)
{
   eval_right_shift(t.base_data(), v.base_data(), static_cast<unsigned long>(i));
   eval_redc(t.base_data(), t.mod_data());
}

template <class Backend>
inline void eval_bitwise_and(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& v)
{
   eval_bitwise_and(result.base_data(), result.base_data(), v.base_data());
   eval_redc(result.base_data(), result.mod_data());
}

template <class Backend>
inline void eval_bitwise_or(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& v)
{
   eval_bitwise_or(result.base_data(), result.base_data(), v.base_data());
}

template <class Backend>
inline void eval_bitwise_xor(modular_adaptor<Backend>& result, const modular_adaptor<Backend>& v)
{
   eval_bitwise_xor(result.base_data(), result.base_data(), v.base_data());
   eval_redc(result.base_data(), result.mod_data());
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
