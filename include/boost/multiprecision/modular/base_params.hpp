//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_BASE_PARAMS_HPP
#define BOOST_MULTIPRECISION_BASE_PARAMS_HPP

namespace boost {
namespace multiprecision {
namespace backends {

template <typename Backend>
class base_params
{
   typedef number<Backend> number_type;

 protected:
   template <typename Number>
   void initialize_base_params(const Number& mod)
   {
      m_mod = mod;
   }

 public:
   base_params() {}

   template <typename Number>
   explicit base_params(const Number& p)
   {
      initialize_base_params(p);
   }

   const number_type& mod() const { return m_mod; }

 protected:
   number_type m_mod;
};
}
}
} // namespace boost::multiprecision::backends
#endif //BOOST_MULTIPRECISION_BASE_PARAMS_HPP
