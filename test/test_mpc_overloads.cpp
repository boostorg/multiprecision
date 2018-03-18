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
  Complex I1 = I0/2 + yb*h;

  I1 = I0;

  Complex z{2, 3};
  typename Complex::value_type theta = 0.2;
  int n = 2;
  using std::sin;
  Complex arg = z*sin(theta) - n*theta;
}

int main()
{
  test_overloads<mpc_complex_100>();
}
