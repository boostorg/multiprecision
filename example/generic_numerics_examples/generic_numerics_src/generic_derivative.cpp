template<typename value_type,
         typename function_type>
value_type derivative(const value_type x,
                      const value_type dx,
                      function_type func)
{
  // Compute d/dx[func(*first)] using a three-point
  // central difference rule of O(dx^6).

  const value_type dx1 = dx;
  const value_type dx2 = dx1 * 2;
  const value_type dx3 = dx1 * 3;

  const value_type m1 = (  func(x + dx1)
                         - func(x - dx1)) / 2;
  const value_type m2 = (  func(x + dx2)
                         - func(x - dx2)) / 4;
  const value_type m3 = (  func(x + dx3)
                         - func(x - dx3)) / 6;

  const value_type fifteen_m1 = 15 * m1;
  const value_type six_m2     =  6 * m2;
  const value_type ten_dx1    = 10 * dx1;

  return ((fifteen_m1 - six_m2) + m3) / ten_dx1;
}

#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/constants/constants.hpp>

using boost::math::constants::pi;
using boost::multiprecision::cpp_dec_float_50;

int main(int, char**)
{
  const float d_f =
    derivative(float(pi<float>() / 3),
               0.01F,
               [](const float x) -> float
               {
                 return ::sin(x);
               });

  const double d_d =
    derivative(double(pi<double>() / 3),
               0.001,
               [](const double x) -> double
               {
                 return ::sin(x);
               });

  const cpp_dec_float_50 d_mp =
    derivative(cpp_dec_float_50(pi<cpp_dec_float_50>() / 3),
               cpp_dec_float_50(1.0E-9),
               [](const cpp_dec_float_50 x) -> cpp_dec_float_50
               {
                 return boost::multiprecision::sin(x);
               });

  // 0.500003
  std::cout
    << std::setprecision(std::numeric_limits<float>::digits10)
    << d_f
    << std::endl;

  // 0.499999999999888
  std::cout
    << std::setprecision(std::numeric_limits<double>::digits10)
    << d_d
    << std::endl;

  // 0.50000000000000000000000000000000000000000003925935
  std::cout
    << std::setprecision(std::numeric_limits<cpp_dec_float_50>::digits10)
    << d_mp
    << std::endl;
}
