template<typename value_type,
         typename function_type>
inline value_type integral(const value_type a,
                           const value_type b,
                           const value_type tol,
                           function_type func)
{
  unsigned n = 1U;

  value_type h = (b - a);
  value_type I = (func(a) + func(b)) * (h / 2);

  for(unsigned k = 0U; k < 8U; k++)
  {
    h /= 2;

    value_type sum(0);
    for(unsigned j = 1U; j <= n; j++)
    {
      sum += func(a + (value_type((j * 2) - 1) * h));
    }

    const value_type I0 = I;
    I = (I / 2) + (h * sum);

    const value_type ratio     = I0 / I;
    const value_type delta     = ratio - 1;
    const value_type delta_abs = ((delta < 0) ? -delta : delta);

    if((k > 1U) && (delta_abs < tol))
    {
      break;
    }

    n *= 2U;
  }

  return I;
}

#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/constants/constants.hpp>

template<typename value_type>
class cyl_bessel_j_integral_rep
{
public:
  cyl_bessel_j_integral_rep(const unsigned N,
                            const value_type& X) : n(N), x(X) { }

  value_type operator()(const value_type& t) const
  {
    // pi * Jn(x) = Int_0^pi [cos(x * sin(t) - n*t) dt]
    return cos(x * sin(t) - (n * t));
  }

private:
  const unsigned n;
  const value_type x;
};

using boost::math::constants::pi;
typedef boost::multiprecision::cpp_dec_float_50 mp_type;

int main(int, char**)
{
  const float j2_f =
    integral(0.0F,
             pi<float>(),
             0.01F,
             cyl_bessel_j_integral_rep<float>(2U, 1.23F)) / pi<float>();

  const double j2_d =
    integral(0.0,
             pi<double>(),
             0.0001,
             cyl_bessel_j_integral_rep<double>(2U, 1.23)) / pi<double>();

  const mp_type j2_mp =
    integral(mp_type(0),
             pi<mp_type>(),
             mp_type(1.0E-20),
             cyl_bessel_j_integral_rep<mp_type>(2U, mp_type(123) / 100)) / pi<mp_type>();

  // 0.166369
  std::cout
    << std::setprecision(std::numeric_limits<float>::digits10)
    << j2_f
    << std::endl;

  // 0.166369383786814
  std::cout
    << std::setprecision(std::numeric_limits<double>::digits10)
    << j2_d
    << std::endl;

  // 0.16636938378681407351267852431513159437103348245333
  std::cout
    << std::setprecision(std::numeric_limits<mp_type>::digits10)
    << j2_mp
    << std::endl;
}
