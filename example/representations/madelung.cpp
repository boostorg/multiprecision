#include <boost/multiprecision/mpfr.hpp>
#include "analyze.hpp"

template<typename Real>
Real madelung()
{
  // Computed in Mathematica using:
  /*
  Quiet[12 Pi (Sech[Pi/Sqrt[2]]^2 + 
  NSum[Sum[
  Sech[Pi Norm[2 v + 1]/2]^2, {v, 
    FrobeniusSolve[{1, 1}, Round[m]]}, Method -> "Procedural"], {m,
      1, Infinity}, Compiled -> False, Method -> "WynnEpsilon", 
      NSumTerms -> 200, WorkingPrecision -> 500])]
  */
  Real x{"1.7475645946331821906362120355443974034851614366247417581528253507650406235327611798907583626946078899308325815387537105932820299441838280130369330021565993632823766071722975686592380371672038104106034214556064382777786832173132243697558773426250474787821285086056791668167573992447684129703678251857628109371313372076707193197424971581157230969923096692739496577811072226715205474090115068915716583082820050184892117803134673122964985829"};
  return x;
}

int main() {
  using boost::multiprecision::mpfr_float;
  mpfr_float::default_precision(400);
  using Real = mpfr_float;
  Real mad = madelung<Real>();
  analyze(mad, "M");
}
