//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//---------------------------------------------------------------------------//

#include <iostream>
#include <boost/multiprecision/cpp_modular.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_complex.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <type_traits>

using namespace boost::multiprecision;

int main()
{
   cpp_mod_params mod(7);
   cpp_mod x(9, mod);
   std::cout << x << std::endl;

}
