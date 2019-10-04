//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#include <type_traits>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_modular.hpp>
#include <boost/multiprecision/cpp_complex.hpp>
#include <chrono>
using namespace boost::multiprecision;

int main() {
   cpp_int base = 2234234234234;
   cpp_int exp("34534534534532342342342344534524");
   cpp_int exp_1 = exp + 1;
   cpp_int mod = 25234234234234;

   cpp_mod base_m(base, mod);
   cpp_mod exp_m(exp, exp_1);

   auto t1 = std::chrono::high_resolution_clock::now();
   std::cout << powm(base, exp, mod) << std::endl;
   auto t2 = std::chrono::high_resolution_clock::now();
   std::cout << pow(base_mt, exp_mt) << std::endl;
   auto t3 = std::chrono::high_resolution_clock::now();
   auto duration_zerg = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
   auto duration_boost = std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count();

   std::cout << "Zerg=" << duration_zerg << std::endl;
   std::cout << "Boost=" << duration_boost << std::endl;
   return 0;
}