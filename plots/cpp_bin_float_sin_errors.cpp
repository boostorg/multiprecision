
//  (C) Copyright Nick Thompson 2020.
//  (C) Copyright John Maddock 2020.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <iostream>
#include <boost/math/tools/ulps_plot.hpp>
#include <boost/math/special_functions/relative_difference.hpp>
#include <boost/core/demangle.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

using boost::math::tools::ulps_plot;
#if 0
namespace boost { namespace multiprecision { 
   
namespace detail {

template <class T>
struct transcendental_reduction_type
{
   typedef T type;
};

template <unsigned Digits, boost::multiprecision::backends::digit_base_type DigitBase, class Allocator, class Exponent, Exponent MinExponent, Exponent MaxExponent>
struct transcendental_reduction_type<cpp_bin_float<Digits, DigitBase, Allocator, Exponent, MinExponent, MaxExponent> >
{
   typedef cpp_bin_float<cpp_bin_float<Digits, DigitBase, Allocator, Exponent, MinExponent, MaxExponent>::bit_count * 3, digit_base_2, Allocator, Exponent, MinExponent, MaxExponent> type;
};

}
   
namespace default_ops {

template <class T>
void reduce_n_half_pi(T& arg, const T& n, bool go_down)
{
   typedef typename boost::multiprecision::detail::transcendental_reduction_type<T>::type reduction_type;

   reduction_type big_arg(arg);
   reduction_type reduction = get_constant_pi<reduction_type>();
   eval_ldexp(reduction, reduction, -1); // divide by 2
   eval_multiply(reduction, n);
   BOOST_MATH_INSTRUMENT_CODE(big_arg.str(10, std::ios_base::scientific));
   BOOST_MATH_INSTRUMENT_CODE(reduction.str(10, std::ios_base::scientific));

   if (go_down)
      eval_subtract(big_arg, reduction, big_arg);
   else
      eval_subtract(big_arg, reduction);
   arg = big_arg;
   BOOST_MATH_INSTRUMENT_CODE(big_arg.str(10, std::ios_base::scientific));
   BOOST_MATH_INSTRUMENT_CODE(arg.str(10, std::ios_base::scientific));
}

template <class T>
void _eval_sin(T& result, const T& x)
{
   BOOST_STATIC_ASSERT_MSG(number_category<T>::value == number_kind_floating_point, "The sin function is only valid for floating point types.");
   BOOST_MATH_INSTRUMENT_CODE(x.str(0, std::ios_base::scientific));
   if (&result == &x)
   {
      T temp;
      eval_sin(temp, x);
      result = temp;
      return;
   }

   typedef typename boost::multiprecision::detail::canonical<boost::int32_t, T>::type  si_type;
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;
   typedef typename mpl::front<typename T::float_types>::type                          fp_type;

   switch (eval_fpclassify(x))
   {
   case FP_INFINITE:
   case FP_NAN:
      if (std::numeric_limits<number<T, et_on> >::has_quiet_NaN)
      {
         result = std::numeric_limits<number<T, et_on> >::quiet_NaN().backend();
         errno  = EDOM;
      }
      else
         BOOST_THROW_EXCEPTION(std::domain_error("Result is undefined or complex and there is no NaN for this number type."));
      return;
   case FP_ZERO:
      result = x;
      return;
   default:;
   }

   // Local copy of the argument
   T xx = x;

   // Analyze and prepare the phase of the argument.
   // Make a local, positive copy of the argument, xx.
   // The argument xx will be reduced to 0 <= xx <= pi/2.
   bool b_negate_sin = false;

   if (eval_get_sign(x) < 0)
   {
      xx.negate();
      b_negate_sin = !b_negate_sin;
   }

   T n_pi, t;
   T half_pi = get_constant_pi<T>();
   eval_ldexp(half_pi, half_pi, -1); // divide by 2
   // Remove multiples of pi/2.
   if (xx.compare(half_pi) > 0)
   {
      eval_divide(n_pi, xx, half_pi);
      eval_trunc(n_pi, n_pi);
      t = ui_type(4);
      eval_fmod(t, n_pi, t);
      bool b_go_down = false;
      if (t.compare(ui_type(1)) == 0)
      {
         b_go_down = true;
      }
      else if (t.compare(ui_type(2)) == 0)
      {
         b_negate_sin = !b_negate_sin;
      }
      else if (t.compare(ui_type(3)) == 0)
      {
         b_negate_sin = !b_negate_sin;
         b_go_down    = true;
      }

      if (b_go_down)
         eval_increment(n_pi);
      //
      // If n_pi is > 1/epsilon, then it is no longer an exact integer value
      // but an approximation.  As a result we can no longer reliably reduce
      // xx to 0 <= xx < pi/2, nor can we tell the sign of the result as we need
      // n_pi % 4 for that, but that will always be zero in this situation.
      // We could use a higher precision type for n_pi, along with division at
      // higher precision, but that's rather expensive.  So for now we do not support
      // this, and will see if anyone complains and has a legitimate use case.
      //
      if (n_pi.compare(get_constant_one_over_epsilon<T>()) > 0)
      {
         BOOST_THROW_EXCEPTION(std::runtime_error("Argument reduction failed in call to sin"));
         result = ui_type(0);
         return;
      }

      reduce_n_half_pi(xx, n_pi, b_go_down);

      BOOST_MATH_INSTRUMENT_CODE(xx.str(0, std::ios_base::scientific));
      BOOST_MATH_INSTRUMENT_CODE(n_pi.str(0, std::ios_base::scientific));
      BOOST_ASSERT(xx.compare(half_pi) < 0);
      BOOST_ASSERT(xx.compare(ui_type(0)) >= 0);
   }

   t = half_pi;
   eval_subtract(t, xx);

   const bool b_zero    = eval_get_sign(xx) == 0;
   const bool b_pi_half = eval_get_sign(t) == 0;

   BOOST_MATH_INSTRUMENT_CODE(xx.str(0, std::ios_base::scientific));
   BOOST_MATH_INSTRUMENT_CODE(t.str(0, std::ios_base::scientific));

   // Check if the reduced argument is very close to 0 or pi/2.
   const bool b_near_zero    = xx.compare(fp_type(1e-1)) < 0;
   const bool b_near_pi_half = t.compare(fp_type(1e-1)) < 0;

   if (b_zero)
   {
      result = ui_type(0);
   }
   else if (b_pi_half)
   {
      result = ui_type(1);
   }
   else if (b_near_zero)
   {
      eval_multiply(t, xx, xx);
      eval_divide(t, si_type(-4));
      T t2;
      t2 = fp_type(1.5);
      hyp0F1(result, t2, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
      eval_multiply(result, xx);
   }
   else if (b_near_pi_half)
   {
      eval_multiply(t, t);
      eval_divide(t, si_type(-4));
      T t2;
      t2 = fp_type(0.5);
      hyp0F1(result, t2, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
   }
   else
   {
      // Scale to a small argument for an efficient Taylor series,
      // implemented as a hypergeometric function. Use a standard
      // divide by three identity a certain number of times.
      // Here we use division by 3^9 --> (19683 = 3^9).

      static const si_type n_scale           = 9;
      static const si_type n_three_pow_scale = static_cast<si_type>(19683L);

      eval_divide(xx, n_three_pow_scale);

      // Now with small arguments, we are ready for a series expansion.
      eval_multiply(t, xx, xx);
      eval_divide(t, si_type(-4));
      T t2;
      t2 = fp_type(1.5);
      hyp0F1(result, t2, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
      eval_multiply(result, xx);

      // Convert back using multiple angle identity.
      for (boost::int32_t k = static_cast<boost::int32_t>(0); k < n_scale; k++)
      {
         // Rescale the cosine value using the multiple angle identity.
         eval_multiply(t2, result, ui_type(3));
         eval_multiply(t, result, result);
         eval_multiply(t, result);
         eval_multiply(t, ui_type(4));
         eval_subtract(result, t2, t);
      }
   }

   if (b_negate_sin)
      result.negate();
   BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
}


}}} // namespace boost::multiprecision::default_ops
#endif
int main() {
   using PreciseReal = boost::multiprecision::mpfr_float_100;
   using CoarseReal = boost::multiprecision::cpp_bin_float_50;

   typedef boost::math::policies::policy<
      boost::math::policies::promote_float<false>,
      boost::math::policies::promote_double<false> >
      no_promote_policy;

   auto ai_coarse = [](CoarseReal const& x)->CoarseReal {
      return sin(x);
   };
   auto ai_precise = [](PreciseReal const& x)->PreciseReal {
      return sin(x);
   };
   /*
   for (CoarseReal val = 1e45; ; val *= 4)
   {
      CoarseReal found = ai_coarse(val);
      CoarseReal expect = CoarseReal(ai_precise(PreciseReal(val)));
      std::cout << std::setw(30) << val << std::setw(30) << boost::math::relative_difference(found, expect) << std::endl;
   }
   */
   std::string filename = "cpp_bin_float_sin.svg";
   int samples = 100000;
   // How many pixels wide do you want your .svg?
   int width = 700;
   // Near a root, we have unbounded relative error. So for functions with roots, we define an ULP clip:
   PreciseReal clip = 50;
   // Should we perturb the abscissas? i.e., should we compute the high precision function f at x,
   // and the low precision function at the nearest representable x̂ to x?
   // Or should we compute both the high precision and low precision function at a low precision representable x̂?
   bool perturb_abscissas = false;
   auto plot = ulps_plot<decltype(ai_precise), PreciseReal, CoarseReal>(ai_precise, CoarseReal(-20), CoarseReal(20), samples, perturb_abscissas);
   // Note the argument chaining:
   plot./*clip(clip).*/width(width);
   plot.background_color("white").font_color("black");
   // Sometimes it's useful to set a title, but in many cases it's more useful to just use a caption.
   //std::string title = "Airy Ai ULP plot at " + boost::core::demangle(typeid(CoarseReal).name()) + " precision";
   //plot.title(title);
   plot.vertical_lines(6);
   plot.add_fn(ai_coarse);
   // You can write the plot to a stream:
   //std::cout << plot;
   // Or to a file:
   plot.write(filename);
}
