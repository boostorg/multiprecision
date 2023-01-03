#include <iomanip>
#include <iostream>

// g++ -O3 -Wall -march=native -std=c++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -o test_funcs.exe
// g++ -O3 -Wall -march=native -std=gnu++11 -DBOOST_MATH_USE_FLOAT128 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -lquadmath -o test_funcs.exe

#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/multiprecision/cpp_double_fp.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

template<typename MpFloatType>
void represent_cyl_bessel_j()
{
   // N[BesselJ[2, 345/100], 101]
   // 0.46452112540537213897844513503677773921598161558478057526782559731407738667745222063644605126028883049

   std::cout << std::endl << "represent_cyl_bessel_j" << std::endl;

   using float_type = MpFloatType;

   const float_type b    = boost::math::cyl_bessel_j(2, float_type(345) / 100);
   const float_type ctrl("0.46452112540537213897844513503677773921598161558478057526782559731407738667745222063644605126028883049");

   const float_type delta = fabs(1 - (b / ctrl));

   const std::streamsize original_streamsize = std::cout.precision();
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << b    << std::endl;
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << ctrl << std::endl;
   std::cout << std::scientific << std::setprecision(4) << delta << std::endl;
   std::cout.precision(original_streamsize);
   std::cout.unsetf(std::ios::scientific);
}

template<typename MpFloatType>
void represent_tgamma_half()
{
   // N[Sqrt[Pi], 101]
   // 1.7724538509055160272981674833411451827975494561223871282138077898529112845910321813749506567385446654

   std::cout << std::endl << "represent_tgamma_half" << std::endl;

   using float_type = MpFloatType;

   const float_type g    = boost::math::tgamma(float_type(0.5F));
   const float_type ctrl = sqrt(boost::math::constants::pi<float_type>());

   const float_type delta = fabs(1 - (g / ctrl));

   const std::streamsize original_streamsize = std::cout.precision();
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << g    << std::endl;
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << ctrl << std::endl;
   std::cout << std::scientific << std::setprecision(4) << delta << std::endl;
   std::cout.precision(original_streamsize);
   std::cout.unsetf(std::ios::scientific);
}

int main()
{
   {
      using double_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<float>, boost::multiprecision::et_off>;
      using dec_float_type    = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<double_float_type>::digits10>, boost::multiprecision::et_off>;

      const double_float_type lg = log(double_float_type(602) / double_float_type(100));
      const double_float_type ep = exp(double_float_type(602) / double_float_type(100));

      std::cout << std::setprecision(std::numeric_limits<double_float_type>::digits10) << lg << std::endl;
      std::cout << std::setprecision(std::numeric_limits<double_float_type>::digits10) << ep << std::endl;

      represent_tgamma_half<double_float_type>();
      represent_tgamma_half<dec_float_type>();

      represent_cyl_bessel_j<double_float_type>();
      represent_cyl_bessel_j<dec_float_type>();
   }

   {
      using double_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<double>, boost::multiprecision::et_off>;
      using dec_float_type    = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<double_float_type>::digits10>, boost::multiprecision::et_off>;

      represent_tgamma_half<double_float_type>();
      represent_tgamma_half<dec_float_type>();

      represent_cyl_bessel_j<double_float_type>();
      represent_cyl_bessel_j<dec_float_type>();
   }

   {
      using double_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<long double>, boost::multiprecision::et_off>;
      using dec_float_type    = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<double_float_type>::digits10>, boost::multiprecision::et_off>;

      represent_tgamma_half<double_float_type>();
      represent_tgamma_half<dec_float_type>();

      represent_cyl_bessel_j<double_float_type>();
      represent_cyl_bessel_j<dec_float_type>();
   }

   #if defined(BOOST_MATH_USE_FLOAT128)
   {
      using double_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<boost::multiprecision::float128>, boost::multiprecision::et_off>;
      using dec_float_type    = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<double_float_type>::digits10>, boost::multiprecision::et_off>;

      represent_tgamma_half<double_float_type>();
      represent_tgamma_half<dec_float_type>();

      represent_cyl_bessel_j<double_float_type>();
      represent_cyl_bessel_j<dec_float_type>();
   }
   #endif
}
