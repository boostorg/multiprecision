//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

//[cpp_modular_eg
#include <iostream>
#include <boost/multiprecision/cpp_modular.hpp>

template <class Modular, class Params>
void modular_number_examples()
{
   std::cout << "Pre-calculation parameters for module: " << std::endl;
   Params mod(7);
   std::cout << "Value mod: " << mod << std::endl;
   Modular a(4, 7), b(4, mod), c(9, mod), d(3, 4);
   std::cout << "Initialization a equal b: " << (a == b) << std::endl;
   std::cout << "Value a: " << a << std::endl;
   std::cout << "Value b: " << b << std::endl;
   std::cout << "Value c: " << c << std::endl;

   std::cout << "Some base function: " << std::endl;
   std::cout << "Add a and b: " << a + b << std::endl;
   std::cout << "Sub a and b: " << a - b << std::endl;
   std::cout << "Sub c and a ((-y mod x) equal ((x-y) mod x): " << c - a << std::endl;
   std::cout << "Multiply a and b: " << a * b << std::endl;
   std::cout << "Divide a and b: " << a / b << std::endl;
   std::cout << "Module a % b: " << a % b << std::endl;

   std::cout << "Some bitwise function: "  << std::endl;
   std::cout << "a and b: " << (a & b) << std::endl;
   std::cout << "a xor b: " << (a ^ b) << std::endl;
   std::cout << "a or b: " << (a | b) << std::endl;

   std::cout << "Pow function: " <<  std::endl;
   std::cout << "Pow a^b: " << pow(a, b) << std::endl;
}

int main()
{
   modular_number_examples<boost::multiprecision::cpp_mod, boost::multiprecision::cpp_mod_params>();
   return 0;
}

//]