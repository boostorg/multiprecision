///////////////////////////////////////////////////////////////
//  # Copyright (c) 2018-2019 Alexey Moskvin

#include <iostream>
#include <boost/multiprecision/modular_adaptor.hpp>
#include <boost/multiprecision/montgomery_params.hpp>
#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

int main()
{
   boost::multiprecision::montgomery_params<cpp_int> tt;
   std::cout << "Hello world" << std::endl;
}
