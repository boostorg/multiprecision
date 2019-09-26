//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#include <iostream>
#include <boost/multiprecision/cpp_modular.hpp>
#include <boost/multiprecision/tommath.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/cpp_complex.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <type_traits>

#include <gmp.h>

using namespace boost::multiprecision;

int main()
{
   mpz_int_mod t(2, 7);
   mpz_int_mod r(9, 7);
   t = 3;
   cpp_int x(2);
   cpp_int y(3);
   x = x & y;
   t = t & r;
   std::cout << x << std::endl;
   std::cout << t << std::endl;

   return 0;
}
