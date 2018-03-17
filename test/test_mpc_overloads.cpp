#include "test.hpp"
#include <boost/multiprecision/mpc.hpp>

using boost::multiprecision::mpc_complex_100;

template<class Complex>
void test_overloads()
{
  Complex ya = {5.2, 7.4};
  Complex yb = {8.2, 7.3};
  typename Complex::value_type h = 0.0001;
  auto I0 = (ya + yb)*h;
  auto I1 = I0/2 + yb*h;

  I1 = I0;
}

int main()
{
  test_overloads<mpc_complex_100>();
}
