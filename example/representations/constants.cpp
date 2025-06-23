#include <boost/multiprecision/mpfr.hpp>
#include "analyze.hpp"
#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
int main() {
  using boost::multiprecision::mpfr_float;
  mpfr_float::default_precision(1000);
  using Real = mpfr_float;
  
  analyze(pi<Real>(), "π");
  analyze(euler<Real>(), "γ");
  analyze(khinchin<Real>(), "K0");
  analyze(glaisher<Real>(), "glaisher");
  analyze(plastic<Real>(), "plastic");
  analyze(gauss<Real>(), "gauss");
  analyze(first_feigenbaum<Real>(), "feigenbaum");
  analyze(rayleigh_kurtosis_excess<Real>(), "rayleigh_excess_kurtosis");
  analyze(rayleigh_skewness<Real>(), "rayleigh_skewness");
  analyze(extreme_value_skewness<Real>(), "extreme_value_skewness");
  analyze(catalan<Real>(), "catalan");
  analyze(zeta_three<Real>(), "zeta_three");
  analyze(sin_one<Real>(), "sin(1)");
  analyze(cos_one<Real>(), "cos(1)");
  analyze(e_pow_pi<Real>(), "e^pi");
  analyze(dottie<Real>(), "d");
  analyze(laplace_limit<Real>(), "λ");
  analyze(reciprocal_fibonacci<Real>(), "ψ");
}
