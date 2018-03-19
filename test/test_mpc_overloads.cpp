#include <type_traits>
#include "test.hpp"
#include <boost/multiprecision/mpc.hpp>

using boost::multiprecision::mpc_complex_100;

template<class Complex>
void test_overloads()
{
  typedef typename Complex::value_type Real;
  Complex ya = {5.2, 7.4};
  Complex yb = {8.2, 7.3};
  Real h = 0.0001;
  auto I0 = (ya + yb)*h;
  Complex I1 = I0/2 + yb*h;

  //I1 = I0;  // not supposed to work.

  Complex z{2, 3};
  typename Complex::value_type theta = 0.2;
  int n = 2;
  using std::sin;
  Complex arg = z*sin(theta) - n*theta;


  using std::exp;
  Real v = 0.2;
  Real cotv = 7.8;
  Real cscv = 8.2;
  Complex den = z + v*cscv*exp(-v*cotv);
}



int main()
{
  test_overloads<mpc_complex_100>();
  
}
