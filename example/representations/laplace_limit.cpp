#include <boost/multiprecision/mpfr.hpp>
#include "analyze.hpp"

template<typename Real>
Real laplace_limit()
{
  using std::cos;
  using std::abs;
  using std::sin;
  using std::sqrt;
  using std::exp;
  Real x{"0.66274341934918158097474209710925290705623354911502241752039253499097185308651127724965480259895818168"};
  Real tmp = sqrt(1+x*x);
  Real etmp = exp(tmp);
  Real residual = x*exp(tmp) - 1 - tmp;
  Real df = etmp -x/tmp + etmp*x*x/tmp;
  do {
    x -= residual/df;
    tmp = sqrt(1+x*x);
    etmp = exp(tmp);
    residual = x*exp(tmp) - 1 - tmp;
    df = etmp -x/tmp + etmp*x*x/tmp;    
  } while(abs(residual) > std::numeric_limits<Real>::epsilon());
  std::cout << "Residual = " << residual << "\n";
  return x;
}

int main() {
  using boost::multiprecision::mpfr_float;
  mpfr_float::default_precision(1300);
  using Real = mpfr_float;
  Real ll = laplace_limit<Real>();
  analyze(ll, "laplace_limit");
}
