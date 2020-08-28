
//  (C) Copyright Nick Thompson 2020.
//  (C) Copyright John Maddock 2020.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <iostream>
#include <boost/math/tools/ulps_plot.hpp>
#include <boost/core/demangle.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

using boost::math::tools::ulps_plot;
#if 0
namespace boost {
namespace multiprecision {
namespace default_ops {

template <class T>
void _eval_cos(T& result, const T& x)
{
   BOOST_STATIC_ASSERT_MSG(number_category<T>::value == number_kind_floating_point, "The cos function is only valid for floating point types.");
   if (&result == &x)
   {
      T temp;
      eval_cos(temp, x);
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
      result = ui_type(1);
      return;
   default:;
   }

   // Local copy of the argument
   T xx = x;

   // Analyze and prepare the phase of the argument.
   // Make a local, positive copy of the argument, xx.
   // The argument xx will be reduced to 0 <= xx <= pi/2.
   bool b_negate_cos = false;

   if (eval_get_sign(x) < 0)
   {
      xx.negate();
   }
   BOOST_MATH_INSTRUMENT_CODE(xx.str(0, std::ios_base::scientific));

   T n_pi, t;
   T half_pi = get_constant_pi<T>();
   eval_ldexp(half_pi, half_pi, -1);  // divide by 2
   // Remove even multiples of pi.
   if (xx.compare(half_pi) > 0)
   {
      eval_divide(t, xx, half_pi);
      eval_trunc(n_pi, t);
      BOOST_MATH_INSTRUMENT_CODE(n_pi.str(0, std::ios_base::scientific));
      t = ui_type(4);
      eval_fmod(t, n_pi, t);

      bool b_go_down = false;
      if (t.compare(ui_type(0)) == 0)
      {
         b_go_down = true;
      }
      else if (t.compare(ui_type(1)) == 0)
      {
         b_negate_cos = true;
      }
      else if (t.compare(ui_type(2)) == 0)
      {
         b_go_down    = true;
         b_negate_cos = true;
      }
      else
      {
         BOOST_ASSERT(t.compare(ui_type(3)) == 0);
      }

      if (b_go_down)
         eval_increment(n_pi);

      eval_multiply(t, n_pi, half_pi);
      BOOST_MATH_INSTRUMENT_CODE(t.str(0, std::ios_base::scientific));
      //
      // If t is so large that all digits cancel the result of this subtraction
      // is completely meaningless, just assume the result is zero for now...
      //
      // TODO We should of course do much better, see:
      // "ARGUMENT REDUCTION FOR HUGE ARGUMENTS" K C Ng 1992
      //
      if (n_pi.compare(get_constant_one_over_epsilon<T>()) > 0)
      {
         result = ui_type(1);
         return;
      }

      if (b_go_down)
      {
         eval_subtract(xx, t, xx);
      }
      else
      {
         eval_subtract(xx, t);
      }

      BOOST_MATH_INSTRUMENT_CODE(xx.str(0, std::ios_base::scientific));
   }
   else
   {
      eval_subtract(xx, half_pi, xx);
   }

   const bool b_zero    = eval_get_sign(xx) == 0;

   // Check if the reduced argument is very close to 0.
   const bool b_near_zero = xx.compare(fp_type(1e-1)) < 0;

   if (b_zero)
   {
      result = si_type(0);
   }/*
   else if (b_pi_half)
   {
      result = si_type(1);
   } 
   else if (b_near_zero)
   {
      eval_multiply(t, xx, xx);
      eval_divide(t, si_type(-4));
      n_pi = fp_type(0.5f);
      hyp0F1(result, n_pi, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
   }*/
   else
   {
      eval_sin(result, xx);
   }
   if (b_negate_cos)
      result.negate();
   BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
}

}}}

#endif
int main() {
   using PreciseReal = boost::multiprecision::mpfr_float_100;
   using CoarseReal = boost::multiprecision::cpp_bin_float_50;

   typedef boost::math::policies::policy<
      boost::math::policies::promote_float<false>,
      boost::math::policies::promote_double<false> >
      no_promote_policy;

   auto ai_coarse = [](CoarseReal const& x)->CoarseReal {
      return cos(x);
   };
   auto ai_precise = [](PreciseReal const& x)->PreciseReal {
      return cos(x);
   };

   std::string filename = "cpp_bin_float_cos.svg";
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
   plot.clip(clip).width(width);
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
