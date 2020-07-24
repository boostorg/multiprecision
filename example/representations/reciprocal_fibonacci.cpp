#include <boost/multiprecision/mpfr.hpp>
#include "analyze.hpp"

template<typename Real>
Real reciprocal_fibonacci()
{
    Real x0 = 1;
    Real x1 = 1;
    Real sum = 2;
    Real diff = 1;
    while (diff > std::numeric_limits<Real>::epsilon()) {
        Real tmp = x1 + x0;
        diff = 1/tmp;
        sum += diff;
        x0 = x1;
        x1 = tmp;
    }
    return sum;
}

int main() {
  using boost::multiprecision::mpfr_float;
  mpfr_float::default_precision(400);
  using Real = mpfr_float;
  Real rf = reciprocal_fibonacci<Real>();
  std::cout << "r = 3.35988566624317755317201130291892717968890513373196848649555381532513031899668338361541621645679008729704\n";
  analyze(rf, "φ");
}
