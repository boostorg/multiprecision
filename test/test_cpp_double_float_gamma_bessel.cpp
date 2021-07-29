#include <iomanip>
#include <iostream>

#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

template <typename MpFloatType>
void represent_cyl_bessel_j()
{
   std::cout << std::endl
             << "represent_cyl_bessel_j" << std::endl;

   using float_type = MpFloatType;

   const float_type b = boost::math::cyl_bessel_j(2, float_type(345) / 100);
   const float_type ctrl("0.46452112540537213897844513503678");

   const float_type delta = fabs(1 - (b / ctrl));

   // N[BesselJ[2, 345/100], 32]
   // 0.46452112540537213897844513503678
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << b << std::endl;
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << ctrl << std::endl;
   std::cout << std::scientific << std::setprecision(4) << delta << std::endl;
}

template <typename MpFloatType>
void represent_tgamma_half()
{
   std::cout << std::endl
             << "represent_tgamma_half" << std::endl;

   using float_type = MpFloatType;

   const float_type g    = boost::math::tgamma(float_type(0.5F));
   const float_type ctrl = sqrt(boost::math::constants::pi<float_type>());

   const float_type delta = fabs(1 - (g / ctrl));

   // 1.77245385090551602729816748334115
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << g << std::endl;
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << ctrl << std::endl;
   std::cout << std::scientific << std::setprecision(4) << delta << std::endl;
}

int main()
{
   using double_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<double>, boost::multiprecision::et_off>;
   using bin_float_type    = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<std::numeric_limits<double_float_type>::digits10>, boost::multiprecision::et_off>;

   represent_tgamma_half<double_float_type>();
   represent_tgamma_half<bin_float_type>();

   represent_cyl_bessel_j<double_float_type>();
   represent_cyl_bessel_j<bin_float_type>();

   return 0;
}