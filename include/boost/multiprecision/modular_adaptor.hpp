//---------------------------------------------------------------------------//
// Copyright (c) 2018 John Maddock
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MODULAR_ADAPTOR_HPP
#define BOOST_MULTIPRECISION_MODULAR_ADAPTOR_HPP

#include <boost/multiprecision/number.hpp>
#include <boost/cstdint.hpp>
#include <boost/multiprecision/detail/digits.hpp>
#include <boost/functional/hash_fwd.hpp>
#include <boost/type_traits/is_complex.hpp>

#include <boost/multiprecision/detail/digits.hpp>
#include <boost/multiprecision/number.hpp>

#include <boost/multiprecision/montgomery/reduce.hpp>
#include <boost/multiprecision/montgomery_params.hpp>

#include <algorithm>
#include <cmath>
#include <complex>

namespace boost {
namespace multiprecision {
namespace backends {

template <typename Backend>
class modular_adapter_base {
 protected:
   Backend m_base;
   montgomery_params<Backend> m_mod;

 public:
   montgomery_params<Backend>& mod_data() { return m_mod; }

   const montgomery_params<Backend>& mod_data() const { return m_mod; }

};

template <>
class modular_adapter_base<gmp_int> {
 protected:
   gmp_int m_base, m_mod;
 public:
   gmp_int& mod_data() { return m_mod; }

   const gmp_int& mod_data() const { return m_mod; }
};

template <typename Backend, template <typename> class base = modular_adapter_base>
class modular_adaptor : public base<Backend> {

 public:

   Backend& base_data() { return this->m_base; }

   Backend const & base_data() const { return this->m_base; }

   typedef typename Backend::signed_types   signed_types;
   typedef typename Backend::unsigned_types unsigned_types;
   //typedef typename Backend::exponent_type  exponent_type;

   modular_adaptor() { }

   modular_adaptor(const int c) { // TODO: resolve type?
      std::cout << "C=" << c << std::endl;
   }

   modular_adaptor(const modular_adaptor& o)
       : base<Backend>::m_base(o.base_data()), base<Backend>::m_mod(o.mod_data()) {}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

   modular_adaptor(modular_adaptor&& o)
       : base<Backend>::m_base(std::move(o.base_data())), base<Backend>::m_mod(std::move(o.mod_data()))
   {}

#endif
   modular_adaptor(const Backend& val, const montgomery_params<Backend>& mod) {
      this->m_base = val;
      this->m_mod = mod;
      if (default_ops::eval_gt(this->m_base, mod.p().backend())) {
         eval_mod_redc(this->m_base, mod.p().backend());
      }
      eval_multiply(this->m_base, mod.R2());
      eval_reduce(this->m_base, mod);
   }

   modular_adaptor(const Backend& val, const Backend& mod)
       : base<Backend>::m_base(val), base<Backend>::m_mod(mod) {
   }

   modular_adaptor(Backend& val, Backend& mod)
       : base<Backend>::m_base(val), base<Backend>::m_mod(mod) {}

   modular_adaptor(const Backend& mod)
       : base<Backend>::m_base(0), base<Backend>::m_mod(mod) {}

   modular_adaptor& operator=(const modular_adaptor& o)
   {
      this->m_base = o.base_data();
      this->m_mod  = o.mod_data();
      return *this;
   }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

   modular_adaptor& operator=(modular_adaptor&& o)

       BOOST_NOEXCEPT
   {
      this->m_base = std::move(o.base_data());
      this->m_mod  = std::move(o.mod_data());
      return *this;
   }
#endif
   template <class V>
   modular_adaptor& operator=(const V& v)
   {
      // TODO: strange operator
      this->m_base = v;
      this->m_mod = 1;

      return *this;
   }

   modular_adaptor& operator=(const char* s)
   {
      // TODO: Think how create mod_adapter from string
      this->m_base = 1;
      this->m_mod = 1;
      return *this;
   }

   int compare(const modular_adaptor& o) const
   {
      // They are either equal or not:
      return (base<Backend>::m_base.compare(o.base_data()) == 0) &&
                     (base<Backend>::m_mod.compare(o.mod_data()) == 0)
                 ? 0
                 : 1;
   }

   template <class T>
   int compare(const T& val) const
   {
      using default_ops::eval_lt;
      return (base<Backend>::m_base.compare(val) == 0) && eval_lt(base<Backend>::m_mod, val) ? 0 : 1;
   }

   void swap(modular_adaptor& o)
   {
      this->base_data().swap(o.base_data());
      this->mod_data().swap(o.mod_data());
   }

   std::string str(std::streamsize dig, std::ios_base::fmtflags f) const
   {
      //TODO: copy
      eval_redc(this->base_data(), this->mod_data());
      return this->base_data().str(dig, f);
   }

};

template <class Backend, class T>
inline typename enable_if<is_arithmetic<T>, bool>

    ::type eval_eq(const modular_adaptor<Backend>& a,
                   const T&                                        b)

        BOOST_NOEXCEPT
{
   return a.compare(b) == 0;
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
   result.mod_data() = b;
   if (default_ops::eval_gt(result.base_data(), result.mod_data().p().backend())) {
      eval_mod_redc(result.base_data(), result.mod_data().p().backend());
   }
   eval_multiply(result.base_data(), result.mod_data().R2().backend());
   eval_redc(result.base_data(), result.mod_data());
}

template <class T, class V>
inline void assign_components(modular_adaptor<gmp_int>& result,
                              const T& a, const V& b)
{
   result.base_data() = a;
   result.mod_data() = b;
   eval_redc(result.base_data(), result.mod_data());
}

//
template <class Backend, class T>
inline void assign_components(modular_adaptor<Backend>& result,
                              const T& a, const montgomery_params<Backend>& b)
{
   result.base_data() = a;
   result.mod_data()  = b;
   if (default_ops::eval_gt(result.base_data(), result.mod_data().p().backend())) {
      eval_mod_redc(result.base_data(), result.mod_data().p().backend());
   }
   eval_multiply(result.base_data(), result.mod_data().R2().backend());
   eval_redc(result.base_data(), result.mod_data());
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

template <class Backend>
inline void eval_pow(modular_adaptor<Backend>&       result,
                     const modular_adaptor<Backend>& b,
                     const modular_adaptor<Backend>& e)
{
   //TODO: Implement this
}

template <class Backend>
inline void eval_exp(modular_adaptor<Backend>&       result,
                     const modular_adaptor<Backend>& arg)
{
//TODO: Implement this
//   using default_ops::eval_exp;
//
//   eval_exp(result.base_data(), arg.base_data());
//   eval_redc(result.base_data(), result.mod_data());
}


}

using boost::multiprecision::backends::modular_adaptor;

template <class Backend>
struct number_category<modular_adaptor<Backend> > : public boost::mpl::int_<boost::multiprecision::number_kind_complex> // TOOD: change to number_kind_modular
{};

template <class Backend, expression_template_option ExpressionTemplates>
struct component_type<number<modular_adaptor<Backend>, ExpressionTemplates> >
{
   typedef number<Backend, ExpressionTemplates> type;
};

}

}

#endif
