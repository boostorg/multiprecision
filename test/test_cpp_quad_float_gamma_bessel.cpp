#include <iomanip>
#include <iostream>

// g++ -O3 -Wall -march=native -std=c++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -o test_funcs.exe
// g++ -O3 -Wall -march=native -std=gnu++11 -DBOOST_MATH_USE_FLOAT128 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -lquadmath -o test_funcs.exe

#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_quad_float.hpp>
#if defined(BOOST_HAS_FLOAT128)
#include <boost/multiprecision/float128.hpp>
#endif

#include <boost/detail/lightweight_test.hpp>
#include "test.hpp"

template<typename MpFloatType>
void represent_cyl_bessel_j()
{
   // N[BesselJ[2, 345/100], 201]
   // 0.464521125405372138978445135036777739215981615584780575267825597314077386677452220636446051260288830486675855798658160994703015754056928405069951198377961012804145261818117591578370224050912116421752978

   std::cout << std::endl << "represent_cyl_bessel_j" << std::endl;

   using float_type = MpFloatType;

   const float_type b    = boost::math::cyl_bessel_j(2, float_type(345) / 100);
   const float_type ctrl("0.464521125405372138978445135036777739215981615584780575267825597314077386677452220636446051260288830486675855798658160994703015754056928405069951198377961012804145261818117591578370224050912116421752978");

   const float_type delta = fabs(1 - (b / ctrl));

   const std::streamsize original_streamsize = std::cout.precision();
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << b    << std::endl;
   std::cout << std::setprecision(std::numeric_limits<float_type>::digits10) << ctrl << std::endl;
   std::cout << std::scientific << std::setprecision(4) << delta << std::endl;
   std::cout.precision(original_streamsize);
   std::cout.unsetf(std::ios::scientific);

   BOOST_CHECK_CLOSE_FRACTION(b, ctrl, std::numeric_limits<float_type>::epsilon() * 40U);
}

template<typename MpFloatType>
void represent_tgamma_half()
{
   // N[Sqrt[Pi], 201]
   // 1.77245385090551602729816748334114518279754945612238712821380778985291128459103218137495065673854466541622682362428257066623615286572442260252509370960278706846203769865310512284992517302895082622893210

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

   BOOST_CHECK_CLOSE_FRACTION(g, ctrl, std::numeric_limits<float_type>::epsilon() * 40U);
}

int main()
{
   {
      using quad_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_fp_backend<double>, boost::multiprecision::et_off>;
      using dec_float_type  = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<quad_float_type>::digits10>, boost::multiprecision::et_off>;

      represent_tgamma_half<quad_float_type>();
      represent_tgamma_half<dec_float_type>();

      represent_cyl_bessel_j<quad_float_type>();
      represent_cyl_bessel_j<dec_float_type>();
   }

   {
      using quad_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_fp_backend<long double>, boost::multiprecision::et_off>;
      using dec_float_type  = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<quad_float_type>::digits10>, boost::multiprecision::et_off>;

      represent_tgamma_half<quad_float_type>();
      represent_tgamma_half<dec_float_type>();

      represent_cyl_bessel_j<quad_float_type>();
      represent_cyl_bessel_j<dec_float_type>();
   }

   #if defined(BOOST_HAS_FLOAT128)
   {
      using quad_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_fp_backend<boost::multiprecision::float128>, boost::multiprecision::et_off>;
      using dec_float_type  = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<quad_float_type>::digits10>, boost::multiprecision::et_off>;

      represent_tgamma_half<quad_float_type>();
      represent_tgamma_half<dec_float_type>();

      represent_cyl_bessel_j<quad_float_type>();
      represent_cyl_bessel_j<dec_float_type>();
   }
   #endif

   return boost::report_errors();
}
