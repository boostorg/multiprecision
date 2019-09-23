//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//---------------------------------------------------------------------------//

#include <iostream>
#include <boost/multiprecision/modular_adaptor.hpp>
//#include <boost/multiprecision/montgomery_params.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_complex.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <type_traits>

using namespace boost::multiprecision;

int main()
{

   cpp_int base(1);
   cpp_int mod("159387639486734986734896734289672398467");
   cpp_int expon(1024);

   montgomery_params<cpp_int_backend<> > modZ(mod);
   number<modular_adaptor<cpp_int_backend<> > > baseZ(2, modZ);
   number<modular_adaptor<cpp_int_backend<> > > resultZ(base, modZ);

   for (int i = 0; i < expon; ++i) {
      base *= 2;
      resultZ = baseZ * resultZ;
   }
   std::cout << base << std::endl;
   std::cout << mod << std::endl;

   number<modular_adaptor<cpp_int_backend<> > > resultX(base, modZ);

   std::cout << "Bad_result=" << resultX << std::endl;

   std::cout << "True_result=" << base % mod << std::endl;

   std::cout << "My_result=" << resultZ << std::endl;

}
