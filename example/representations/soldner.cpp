#include <boost/multiprecision/mpfr.hpp>
#include <boost/math/special_functions/expint.hpp>
#include "analyze.hpp"

// See: https://en.wikipedia.org/wiki/Ramanujan%E2%80%93Soldner_constant
template<typename T>
T soldner()
{
   using boost::math::expint;
   using std::log;
   using std::abs;
   // Initial guess from https://oeis.org/A070769/constant
   T x{"1.45136923488338105028396848589202744949303228364801586309300455766242559575451783565953135771108682884"};
   T y = expint(log(x));
   // If x is the root, then
   // li(x(1+μ)) = xμli'(x) + Ο(μ²) ≈ xμ/log(x),
   // where μ = ε/2 is the unit roundoff, and we know x ≈ 1.4513.
   // Plugging this in gets:
   const T expected_residual = 1.9481078669535834*std::numeric_limits<T>::epsilon();
   // This should that we cannot expect better than ~2ULPs of accuracy using Newton's method.
   // Just a couple extra bits of accuracy should get this recover full precision,
   // but variable precision always causes us pain . . .
   while (abs(y) > expected_residual) {
      // Use li(x) = Ei(ln(x)).
      T ln = log(x);
      y = expint(ln);
      x -= ln*y;
   }
   return x;
}

int main() {
  using boost::multiprecision::mpfr_float;
  mpfr_float::default_precision(1000);
  using Real = mpfr_float;
  Real s = soldner<Real>();
  analyze(s, "μ");
}
