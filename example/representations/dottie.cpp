#include <boost/multiprecision/mpfr.hpp>
#include "analyze.hpp"

template<typename Real>
Real dottie()
{
  using std::cos;
  using std::abs;
  using std::sin;
  Real x{".739085133215160641655312087673873404013411758900757464965680635773284654883547594599376106931766531849801246"};
  Real residual = cos(x) - x;
  do {
    x += residual/(sin(x)+1);
    residual = cos(x) - x;
  } while(abs(residual) > std::numeric_limits<Real>::epsilon());
  return x;
}

int main() {
  using boost::multiprecision::mpfr_float;
  mpfr_float::default_precision(1300);
  using Real = mpfr_float;
  Real dot = dottie<Real>();
  analyze(dot, "dottie");
}
