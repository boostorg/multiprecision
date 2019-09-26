//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_REDUCE_BELOW_HPP
#define BOOST_MULTIPRECISION_REDUCE_BELOW_HPP

namespace boost {
namespace multiprecision {

template <typename Backend>
void eval_reduce_below(Backend& val, const Backend& mod)
{
   using default_ops::eval_lt;

   if (eval_lt(mod, 0))
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument("Reduce_below modulus must be positive"));
   }

   while (eval_lt(mod, val))
   {
      eval_subtract(val, mod);
   }
}

}
} // namespace boost::multiprecision

#endif //BOOST_MULTIPRECISION_REDUCE_BELOW_HPP
