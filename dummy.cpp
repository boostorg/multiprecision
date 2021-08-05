///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Dummy test for cpp_quad_float
//

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_quad_float.hpp>
#include <iostream>
#include <cstdlib>

int main()
{
   //boost::multiprecision::backends::detail::exact_arithmetic<double>::float_tuple t = std::make_tuple(5e-15, 5e-30, -14e-45, 2.65e-60);
   //boost::multiprecision::backends::detail::exact_arithmetic<double>::float_type  e = 3e-67;
   //boost::multiprecision::backends::detail::exact_arithmetic<double>::normalize(t, e);

   //std::pair<double, double> t(5e-12, 3e-1);
   //double                    e(7e-24);
   //boost::multiprecision::backends::detail::exact_arithmetic<double>::sum(t, e);

   //std::cout << std::hexfloat;
   //std::cout << std::get<0>(t) << " " << std::get<1>(t) << " " << std::get<2>(t) << " " << std::get<3>(t);
   //std::cout << t.first << " " << t.second << " " << e;
   //std::cout << std::endl;

  
   boost::multiprecision::backends::cpp_quad_float<double> a(std::make_tuple(0x1.921fb54442d18p+1, 0x1.1a62633145c07p-53, -0x1.f1976b7ed8fbcp-109, 0x1.3b8d3f60d850cp-163));
   //0x1.5bf0a8b0ad9b2p+1 + -0x1.e86a384b7f304p-53 + 0x1.45fe0602f06dbp-107 + 0x1.d8cc5979789dep-162
   boost::multiprecision::backends::cpp_quad_float<double> e(std::make_tuple(0x1.5bf0a8b0ad9b2p+1, -0x1.e86a384b7f304p-53, 0x1.45fe0602f06dbp-107, 0x1.d8cc5979789dep-162));
   std::cout << a.raw_str() << std::endl;
   a -= e;
   std::cout << a.raw_str() << std::endl;
   std::cin.get();
   return 0;
}
