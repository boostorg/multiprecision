// Copyright Christopher Kormanyos
// Copyright John Maddock 2017
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// Contains Quickbook snippets within comments.

//! Example of computing [@https://en.wikipedia.org/wiki/Gauss–Laguerre_quadrature Gauss–Laguerre quadrature].

#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/cbrt.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/noncopyable.hpp>

// Make it easy to test with som multiprecision types with precision chosen below in struct digits_characteristics.
#define CPP_BIN_FLOAT 1
#define CPP_DEC_FLOAT 2
#define CPP_MPFR_FLOAT 3

// #define MP_TYPE CPP_BIN_FLOAT // A boost::multiprecision::cpp_float_bin_ type.
#define MP_TYPE CPP_DEC_FLOAT // A boost::multiprecision::cpp_float_dec_ type
//#define MP_TYPE CPP_MPFR_FLOAT // A boost::multiprecision::cpp_float_mpfr_ type

namespace {
struct digits_characteristics
{ // Choose precision in decimal digits.
 //  static const int digits10     = 300;
   static const int digits10     = 50;
   static const int guard_digits = 6;
   // This determines the size of the table of abscissas and weights,
   // indexing twice total digits10 + guard_digits (if any),
};
} // namespace

// Include a multiprecision type and define the full number type.
#if (MP_TYPE == CPP_BIN_FLOAT)
#include <boost/multiprecision/cpp_bin_float.hpp>
namespace mp = boost::multiprecision;
typedef mp::number<mp::cpp_bin_float<digits_characteristics::digits10 + digits_characteristics::guard_digits>, mp::et_off> mp_type;
#elif (MP_TYPE == CPP_DEC_FLOAT)
#include <boost/multiprecision/cpp_dec_float.hpp>
namespace mp = boost::multiprecision;
typedef mp::number<mp::cpp_dec_float<digits_characteristics::digits10 + digits_characteristics::guard_digits>, mp::et_off> mp_type;
#elif (MP_TYPE == CPP_MPFR_FLOAT)
#include <boost/multiprecision/mpfr.hpp>
namespace mp = boost::multiprecision;
typedef mp::number<mp::mpfr_float_backend<digits_characteristics::digits10 + digits_characteristics::guard_digits>, mp::et_off> mp_type;
#else
#error MP_TYPE is undefined
#endif

#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <exception>

//! Define a laguerre functor.
template <typename T>
class laguerre_function_object
{
 public:
   laguerre_function_object(const int n, const T a) : order(n),
                                                      alpha(a),
                                                      p1(0),
                                                      d2(0) {}

   laguerre_function_object(const laguerre_function_object& other) : order(other.order),
                                                                     alpha(other.alpha),
                                                                     p1(other.p1),
                                                                     d2(other.d2) {}

   ~laguerre_function_object() {}

   T operator()(const T& x) const
   {
      // Calculate (via forward recursion):
      // * the value of the Laguerre function L(n, alpha, x), called (p2),
      // * the value of the derivative of the Laguerre function (d2),
      // * and the value of the corresponding Laguerre function of
      //   previous order (p1).

      // Return the value of the function (p2) in order to be used as a
      // function object with Boost.Math root-finding. Store the values
      // of the Laguerre function derivative (d2) and the Laguerre function
      // of previous order (p1) in class members for later use.

      p1   = T(0);
      T p2 = T(1);
      d2   = T(0);

      T j_plus_alpha(alpha);
      T two_j_plus_one_plus_alpha_minus_x(1 + alpha - x);

      int j;

      const T my_two(2);

      for (j = 0; j < order; ++j)
      {
         const T p0(p1);

         // Set the value of the previous Laguerre function.
         p1 = p2;

         // Use a recurrence relation to compute the value of the Laguerre function.
         p2 = ((two_j_plus_one_plus_alpha_minus_x * p1) - (j_plus_alpha * p0)) / (j + 1);

         ++j_plus_alpha;
         two_j_plus_one_plus_alpha_minus_x += my_two;
      }

      // Set the value of the derivative of the Laguerre function.
      d2 = ((p2 * j) - (j_plus_alpha * p1)) / x;

      // Return the value of the Laguerre function.
      return p2;
   }

   const T& previous() const { return p1; }
   const T& derivative() const { return d2; }

   static bool root_tolerance(const T& a, const T& b)
   {
      using std::abs;

      // The relative tolerance here is: ((a - b) * 2) / (a + b).
      return (abs((a - b) * 2) < ((a + b) * boost::math::tools::epsilon<T>()));
   }

 private:
   const int order;
   const T   alpha;
   mutable T p1;
   mutable T d2;

   laguerre_function_object();

   const laguerre_function_object& operator=(const laguerre_function_object&);
}; // class laguerre_function_object

//! Compute a static table of approximate roots abscissas and weights.
//! \param xi vector containing x values of crossing y-axis.
//! \param wi vector contain weights.
//! \param order order of the Laguerre function (must not exceed -20).
//! \param alpha
//! \param valid true if table of estimates has been created.

template <typename T>
class gauss_laguerre_abscissas_and_weights : private boost::noncopyable
{
 public:
   gauss_laguerre_abscissas_and_weights(const int n, const T a) : order(n),
                                                                  alpha(a),
                                                                  valid(true),
                                                                  xi(),
                                                                  wi()
   {
      if (alpha < -20.0F)
      {
         // TBD: If we ever boostify this, throw a range error here.
         // If so, then also document it in the docs.
         throw std::range_error("Range error: the order of the Laguerre function must exceed -20.0!");
         std::cout << "Range error: the order of the Laguerre function must exceed -20.0!" << std::endl;
      }
      else
      {
         calculate();
      }
   }

   virtual ~gauss_laguerre_abscissas_and_weights() {}

   const std::vector<T>& abscissas() const { return xi; }
   const std::vector<T>& weights() const { return wi; }

   bool get_valid() const { return valid; }

 private:
   const int order;
   const T   alpha;
   bool      valid;

   std::vector<T> xi;
   std::vector<T> wi;

   //! Find approximate roots_estimates and weights, and store them.
   void calculate()
   {
      using std::abs;

      std::cout << "Finding approximate roots..." << std::endl;

      std::vector<boost::math::tuple<T, T> > root_estimates;

      root_estimates.reserve(static_cast<typename std::vector<boost::math::tuple<T, T> >::size_type>(order));

      const laguerre_function_object<T> laguerre_object(order, alpha);

      // Set the initial values of the step size and the running step
      // to be used for finding the estimate of the first root.
      T step_size = 0.01F;
      T step      = step_size;

      T first_laguerre_root = 0.0F; 

      // Assume a root will be found, but set it false if search for root fails.
      bool first_laguerre_root_has_been_found = true;

      if (alpha < -1.0F)
      {
         // Iteratively step through the Laguerre function using a
         // small step-size in order to find a rough estimate of the first zero.

         bool this_laguerre_value_is_negative = (laguerre_object(mp_type(0)) < 0);

         static const int j_max = 10000;

         int j;

         for (j = 0; (j < j_max) && (this_laguerre_value_is_negative != (laguerre_object(step) < 0)); ++j)
         {
            // Increment the step size until the sign of the Laguerre function
            // switches. This indicates a zero-crossing, signalling the next root.
            step += step_size;
         }

         if (j >= j_max)
         {
            first_laguerre_root_has_been_found = false;  
         }
         else
         {
            // We have found the first zero-crossing. Put a loose bracket around
            // the root using a window. Here, we know that the first root lies
            // between (x - step_size) < root < x.

            // Before storing the approximate root, perform a couple of
            // bisection steps in order to tighten up the root bracket.
            boost::uintmax_t a_couple_of_iterations = 3U;
            const std::pair<T, T>
                first_laguerre_roots = boost::math::tools::bisect(laguerre_object,
                                                                  step - step_size,
                                                                  step,
                                                                  laguerre_function_object<T>::root_tolerance,
                                                                  a_couple_of_iterations);
            // Set 1st root to be mid-point of bisection pair.
            first_laguerre_root = (first_laguerre_roots.first + first_laguerre_roots.second) / 2;
            static_cast<void>(a_couple_of_iterations); // ???? discard ???
         }
      }
      else
      {
         // Calculate an estimate of the 1st root of a generalized Laguerre
         // function using either a Taylor series or an expansion in Bessel
         // function zeros. The Bessel function zeros expansion is from Tricomi.

         // Here, we obtain an estimate of the first zero of J_alpha(x).

         T j_alpha_m1;

         if (alpha < 1.4F)
         {
            // For small alpha, use a short series obtained from Mathematica(R).
            // Series[BesselJZero[v, 1], {v, 0, 3}]
            // N[%, 12]
            j_alpha_m1 = (((0.09748661784476F * alpha - 0.17549359276115F) * alpha + 1.54288974259931F) * alpha + 2.40482555769577F);
         }
         else
         {
            // For larger alpha >1.4, Asymptotic Expansions for Large Order [@https://dlmf.nist.gov/10.21.vii]
           // use the first line of Eqs. 10.21.40 in the NIST Handbook.
            // [@https://dlmf.nist.gov/10.21.E40]
            const T alpha_pow_third(boost::math::cbrt(alpha));
            const T alpha_pow_minus_two_thirds(T(1) / (alpha_pow_third * alpha_pow_third));

            j_alpha_m1 = alpha * (((((+0.043F * alpha_pow_minus_two_thirds - 0.0908F) * alpha_pow_minus_two_thirds - 0.00397F) * alpha_pow_minus_two_thirds + 1.033150F) * alpha_pow_minus_two_thirds + 1.8557571F) * alpha_pow_minus_two_thirds + 1.0F);
         }

         const T vf             = ((order * 4.0F) + (alpha * 2.0F) + 2.0F);
         const T vf2            = vf * vf;
         const T j_alpha_m1_sqr = j_alpha_m1 * j_alpha_m1;

         first_laguerre_root = (j_alpha_m1_sqr * (-0.6666666666667F + ((0.6666666666667F * alpha) * alpha) + (0.3333333333333F * j_alpha_m1_sqr) + vf2)) / (vf2 * vf);
      }

      if (first_laguerre_root_has_been_found)
      {
         bool this_laguerre_value_is_negative = (laguerre_object(mp_type(0)) < 0);

         // Re-set the initial value of the step-size based on the
         // estimate of the first root.
         step_size = first_laguerre_root / 2;
         step      = step_size;

         // Step through the Laguerre function using a step-size
         // of dynamic width in order to find the zero crossings
         // of the Laguerre function, providing rough estimates
         // of the roots. Refine the brackets with a few bisection steps,
         // and store the results as pairs of bracketed root estimates.

         while (static_cast<int>(root_estimates.size()) < order)
         {
            // Increment the step size until the sign of the Laguerre function
            // switches. This indicates a zero-crossing, signalling the next root.
            step += step_size;

            if (this_laguerre_value_is_negative != (laguerre_object(step) < 0))
            {
               // We have found the next zero-crossing.

               // Change the running sign of the Laguerre function.
               this_laguerre_value_is_negative = (!this_laguerre_value_is_negative);

               // We have found the first zero-crossing. Put a loose bracket around
               // the root using a window. Here, we know that the first root lies
               // between (x - step_size) < root < x.

               // Before storing the approximate root, perform a couple of
               // bisection steps in order to tighten up the root bracket.
               boost::uintmax_t a_couple_of_iterations = 3U;
               const std::pair<T, T>
                   root_estimate_bracket = boost::math::tools::bisect(laguerre_object,
                                                                      step - step_size,
                                                                      step,
                                                                      laguerre_function_object<T>::root_tolerance,
                                                                      a_couple_of_iterations);

               static_cast<void>(a_couple_of_iterations);

               // Store the refined root estimate as a bracketed range in a tuple.
               root_estimates.push_back(boost::math::tuple<T, T>(root_estimate_bracket.first,
                                                                 root_estimate_bracket.second));

               if (root_estimates.size() >= static_cast<std::size_t>(2U))
               {
                  // Determine the next step size. This is based on the distance between
                  // the previous two roots, whereby the estimates of the previous roots
                  // are computed by taking the average of the lower and upper range of
                  // the root-estimate bracket.

                  const T r0 = (boost::math::get<0>(*(root_estimates.rbegin() + 1U)) + boost::math::get<1>(*(root_estimates.rbegin() + 1U))) / 2;

                  const T r1 = (boost::math::get<0>(*root_estimates.rbegin()) + boost::math::get<1>(*root_estimates.rbegin())) / 2;

                  const T distance_between_previous_roots = r1 - r0;

                  step_size = distance_between_previous_roots / 3;
               }
            }
         }

         const T norm_g =
             ((alpha == 0) ? T(-1)
                           : -boost::math::tgamma(alpha + order) / boost::math::factorial<T>(order - 1));

         xi.reserve(root_estimates.size());
         wi.reserve(root_estimates.size());

         // Calculate the abscissas and weights to full precision of type T.
         for (std::size_t i = static_cast<std::size_t>(0U); i < root_estimates.size(); ++i)
         {
            std::cout << "Calculating abscissa and weight for index: " << i << std::endl;

            // Calculate the abscissas using iterative root-finding.

            // Select the maximum allowed iterations, being at least 20.
            // The determination of the maximum allowed iterations is
            // based on the number of decimal digits in the numerical type T.
            const int my_digits10 = static_cast<int>(static_cast<float>(boost::math::tools::digits<T>()) * 0.301F);
            const boost::uintmax_t number_of_iterations_allowed = (std::max)(20, my_digits10 / 2);

            boost::uintmax_t number_of_iterations_used = number_of_iterations_allowed;

            // Perform the root-finding using ACM TOMS 748 from Boost.Math.

            const std::pair<T, T>
                laguerre_root_bracket = boost::math::tools::toms748_solve(laguerre_object,
                                                                          boost::math::get<0>(root_estimates[i]),
                                                                          boost::math::get<1>(root_estimates[i]),
                                                                          laguerre_function_object<T>::root_tolerance,
                                                                          number_of_iterations_used);

            // Based on the result of *each* root-finding operation, re-assess
            // the validity of the Gauss-Laguerre abscissas and weights object.
            valid &= (number_of_iterations_used < number_of_iterations_allowed);

            // Compute the Laguerre root as the average of the values from
            // the solved root bracket.
            const T laguerre_root = (laguerre_root_bracket.first + laguerre_root_bracket.second) / 2;

            // Calculate the weight for this Laguerre root.
            // Here, we calculate the derivative of the Laguerre function and the value of the
            // previous Laguerre function on the x-axis at the value of this Laguerre root.
            static_cast<void>(laguerre_object(laguerre_root));

            // Store the abscissa and weight for this index.
            xi.push_back(laguerre_root);
            wi.push_back(norm_g / ((laguerre_object.derivative() * order) * laguerre_object.previous()));
         }
      }
   }
}; // class gauss_laguerre_abscissas_and_weights


//! Compute airy Ai

namespace {
template <typename T>
struct gauss_laguerre_ai
{
 public:
   gauss_laguerre_ai(const T X) : x(X)
   {
      using std::exp;
      using std::sqrt;

      zeta = ((sqrt(x) * x) * 2) / 3;

      const T zeta_times_48_pow_sixth = sqrt(boost::math::cbrt(zeta * 48));
      // sqrt(boost::math::constants::root_pi<T>() or
      // boost::math::constants::one_div_root_pi<T> perhaps? 
      factor = 1 / ((sqrt(boost::math::constants::pi<T>()) * zeta_times_48_pow_sixth) * 
        (exp(zeta) * gamma_of_five_sixths()));
   }

   gauss_laguerre_ai(const gauss_laguerre_ai& other) : x(other.x),
                                                       zeta(other.zeta),
                                                       factor(other.factor) {}

   T operator()(const T& t) const
   {
      using std::sqrt;

      return factor / sqrt(boost::math::cbrt(2 + (t / zeta)));
   }

 private:
   const T x;
   T       zeta;
   T       factor;

   static const T& gamma_of_five_sixths()
   {
      static const T value = boost::math::tgamma(T(5) / 6);

      return value;
   }

   const gauss_laguerre_ai& operator=(const gauss_laguerre_ai&);
};  // struct gauss_laguerre_ai

template <typename T>
T gauss_laguerre_airy_ai(const T x)
{
   static const float digits_factor  = static_cast<float>(std::numeric_limits<mp_type>::digits10) / 300.0F;
   static const int   laguerre_order = static_cast<int>(600.0F * digits_factor);

   static const gauss_laguerre_abscissas_and_weights<T> abscissas_and_weights(laguerre_order, -T(1) / 6);

   T airy_ai_result;

   if (abscissas_and_weights.get_valid())
   {
      const gauss_laguerre_ai<T> this_gauss_laguerre_ai(x);

      airy_ai_result =
          std::inner_product(abscissas_and_weights.abscissas().begin(),
                             abscissas_and_weights.abscissas().end(),
                             abscissas_and_weights.weights().begin(),
                             T(0),
                             std::plus<T>(),
                             [&this_gauss_laguerre_ai](const T& this_abscissa, const T& this_weight) -> T {
                                return this_gauss_laguerre_ai(this_abscissa) * this_weight;
                             });
   }
   else
   {
      std::cout << "No valid abscissas and weights have been computed!" << std::endl;
      airy_ai_result = T(0);
   }

   return airy_ai_result;
} // gauss_laguerre_airy_ai
} // namespace

int main()
{
   // Use Gauss-Laguerre integration to compute airy_ai(120 / 7).

   // 9 digits
   // 3.89904210e-22

   // 10 digits
   // 3.899042098e-22

   // 50 digits.
   // 3.8990420982303275013276114626640705170145070824318e-22

   // 100 digits.
   // 3.899042098230327501327611462664070517014507082431797677146153303523108862015228
   // 864136051942933142648e-22

   // 200 digits.
   // 3.899042098230327501327611462664070517014507082431797677146153303523108862015228
   // 86413605194293314264788265460938200890998546786740097437064263800719644346113699
   // 77010905030516409847054404055843899790277e-22

   // 300 digits.
   // 3.899042098230327501327611462664070517014507082431797677146153303523108862015228
   // 86413605194293314264788265460938200890998546786740097437064263800719644346113699
   // 77010905030516409847054404055843899790277083960877617919088116211775232728792242
   // 9346416823281460245814808276654088201413901972239996130752528e-22

   // 500 digits.
   // 3.899042098230327501327611462664070517014507082431797677146153303523108862015228
   // 86413605194293314264788265460938200890998546786740097437064263800719644346113699
   // 77010905030516409847054404055843899790277083960877617919088116211775232728792242
   // 93464168232814602458148082766540882014139019722399961307525276722937464859521685
   // 42826483602153339361960948844649799257455597165900957281659632186012043089610827
   // 78871305322190941528281744734605934497977375094921646511687434038062987482900167
   // 45127557400365419545e-22

   // 611 digits.
   // 3.899042098230327501327611462664070517014507082431797677146153303523108862015228
   // 86413605194293314264788265460938200890998546786740097437064263800719644346113699
   // 77010905030516409847054404055843899790277083960877617919088116211775232728792242
   // 9346416823281460245814808276654088201413901972239996130752528e-22

   // Mathematica(R) or Wolfram's Alpha:
   // N[AiryAi[120 / 7], 300]
   // https://www.wolframalpha.com/input/?i=N%5BAiryAi%5B120+%2F+7%5D%2C+300%5D
   // 3.899042098230327501327611462664070517014507082431797677146153303523108862015228
   // 86413605194293314264788265460938200890998546786740097437064263800719644346113699
   // 77010905030516409847054404055843899790277083960877617919088116211775232728792242
   // 9346416823281460245814808276654088201413901972239996130752528×10^-22

   // Agrees exactly with WolframAlpha.

   // N[AiryAi[1/ 10], 300]
   // 0.329203129943538100170199085016430844536476730597544642815477703976493475237515808693095638100575951182527654537484244471409474203135771561202456410206620147589032033387755134578787712439479639654377406851556312354151562284662626461464746132526738582694157114218140891761302188140083085034460467229889
   //
   // 0.329203129935604639291939334231568761918581881613362555725337874660788846322714275027241530894092838093235689300470543425832861218511616582808376169161534490388178933798169026527485191482625248693747471705273063591606787099819657181351283479723817003434354167763174315509380717762950445747705359552525
   //

   // N[AiryAi[100/ 7], 300]

   // N[AiryAi[10/7], 300]
   // 0.0789843815328302634766571099192586677765606222119605023528631703217150659831762747375166388288807621485599901850308911502783472782012305870215650915698545824017353521662412307832942731808689534273082918192060271727434618410938598698983836749367384068959827965440617344745274010840619904303929007866781
   // Airy_ai[10/7]
   // Airy_ai[10 / 7] =
   // 0.0789843815328302634766571099192586677765606222119605023528631703211145656873128286590434476743388982530744824065670681308493466449022942723313037950425610483731304083611936896951385031692566685674427518183720717437833628278336117932632941861331357328374973731377960286945373872012455336932933702223702
   //                                                                     ^ differs at digit 70
   try
   {
      std::cout << "Airy_ai[120/7]=\n"
                << std::setprecision(digits_characteristics::digits10)
                << gauss_laguerre_airy_ai(mp_type(120) / 7) // 120/7 = 1.714285714...
                << std::endl;

      std::cout << "Airy_ai[10/7]=\n"
                << gauss_laguerre_airy_ai(mp_type(10) / 7) // 1.428571429...
                << std::endl;
   }
   catch (std::exception ex)
   {
      std::cout << "Thrown exception " << ex.what() << std::endl;
   }
   return 0;

} // int main()
