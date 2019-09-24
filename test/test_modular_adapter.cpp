//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//---------------------------------------------------------------------------//

#include <iostream>
#include <boost/multiprecision/cpp_modular.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/cpp_complex.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <type_traits>
#include <tommath.h>

using namespace boost::multiprecision;

int main()
{
   montgomery_params<tommath_int> tt(7);
   number<modular_adaptor<tommath_int> > xz(9, tt);
   number<modular_adaptor<tommath_int> > yz(6, tt);

   std::cout << xz << std::endl;
   std::cout << yz << std::endl;
   std::cout << xz + yz << std::endl;
   std::cout << xz * yz << std::endl;

   //mp_toradix_n(&mp, buf, 10, 256); printf("%s\n", buf);
//   mp_montgomery_reduce(&x, &modulus, mp);
//
//   char buf[4096];
//   mp_toradix_n(&x, buf, 10, 256); printf("%s\n", buf);
//   mp_toradix_n(&modulus, buf, 10, 256); printf("%s\n", buf);

   exit(0);
}
