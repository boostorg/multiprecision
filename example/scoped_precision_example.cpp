///////////////////////////////////////////////////////////////
//  Copyright 2021 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <boost/math/special_functions/relative_difference.hpp>
#include <boost/math/special_functions/next.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/debug_adaptor.hpp>

//[scoped_precision_1

/*`
All our precision changing examples are based around `mpfr_float`.
However, in order to make running this example a little easier to debug,
we'll use `debug_adaptor` throughout so that the values of all variables
can be displayed in your debugger of choice:
*/

using mpfr_float = boost::multiprecision::number<boost::multiprecision::debug_adaptor<boost::multiprecision::mpfr_float_backend<0>>>;

/*`
Our first example will investigate calculating the Bessel J function via it's well known 
series representation, this simple series suffers from catestrophic cancellation error
near the roots of the function, so we'll investigate slowly increasing the precision of
the calculation until we get the result to N-decimal places.  We'll begin by defining
a function to calculate the series for Bessel J, the details of which we'll leave in the
source code:
*/
mpfr_float calculate_bessel_J_as_series(mpfr_float x, mpfr_float v, mpfr_float* err)
//<-
{
   mpfr_float sum        = pow(x / 2, v) / tgamma(v + 1);
   mpfr_float abs_sum    = abs(sum);
   mpfr_float multiplier = -x * x / 4;
   mpfr_float divider    = v + 1;
   mpfr_float term       = sum;
   mpfr_float eps        = boost::math::tools::epsilon<mpfr_float>();
   unsigned   m          = 1;

   while (fabs(term / abs_sum) > eps)
   {
      term *= multiplier;
      term /= m;
      term /= divider;
      ++divider;
      ++m;
      sum += term;
      abs_sum += abs(term);
   }
   if (err)
      *err = eps * fabs(abs_sum / sum);
   /*
   std::cout << eps << std::endl;
   if (err)
      std::cout << *err << std::endl;
   std::cout << abs_sum << std::endl;
   std::cout << sum << std::endl;
   */
   return sum;
}
//->

/*`
Next come some simple helper classes, these allow us to modify the current precision and precision-options
via scoped objects which will put everything back as it was at the end.  We'll begin with the class to
modify the working precision:
*/
struct scoped_mpfr_precision
{
   unsigned saved_digits10;
   scoped_mpfr_precision(unsigned digits10) : saved_digits10(mpfr_float::thread_default_precision())
   {
      mpfr_float::thread_default_precision(digits10);
   }
   ~scoped_mpfr_precision()
   {
      mpfr_float::thread_default_precision(saved_digits10);
   }
   void reset(unsigned digits10)
   {
      mpfr_float::thread_default_precision(digits10);
   }
   void reset()
   {
      mpfr_float::thread_default_precision(saved_digits10);
   }
};
/*`
And a second class to modify the precision options:
*/
struct scoped_mpfr_precision_options
{
   boost::multiprecision::variable_precision_options saved_options;
   scoped_mpfr_precision_options(boost::multiprecision::variable_precision_options opts) : saved_options(mpfr_float::thread_default_variable_precision_options())
   {
      mpfr_float::thread_default_variable_precision_options(opts);
   }
   ~scoped_mpfr_precision_options()
   {
      mpfr_float::thread_default_variable_precision_options(saved_options);
   }
   void reset(boost::multiprecision::variable_precision_options opts)
   {
      mpfr_float::thread_default_variable_precision_options(opts);
   }
};
/*`
We can now begin writing a function to calculate J[sub v](z) to a specified precision.  
In order to keep the logic as simple as possible, we'll adopt a ['uniform precision computing] approach, 
which is to say, within the body of the function, all variables are always at the same precision.
*/
mpfr_float Bessel_J_to_precision(mpfr_float v, mpfr_float x, unsigned digits10)
{
   //
   // Begin by backing up digits10:
   //
   unsigned saved_digits10 = digits10;
   // 
   //
   // Start by defining 2 scoped objects to control precision and associated options.
   // We'll begin by setting the working precision to the required target precision,
   // and since all variables will always be of uniform precision, we can tell the
   // library to ignore all precision control by setting variable_precision_options::assume_uniform_precision:
   //
   scoped_mpfr_precision           scoped(digits10);
   scoped_mpfr_precision_options   scoped_opts(boost::multiprecision::variable_precision_options::assume_uniform_precision);

   mpfr_float            result;
   mpfr_float            current_error{1};
   mpfr_float            target_error {std::pow(10., -static_cast<int>(digits10))};

   while (target_error < current_error)
   {
      //
      // Everything must be of uniform precision in here, including
      // our input values, so we'll begin by setting their precision:
      //
      v.precision(digits10);
      x.precision(digits10);
      //
      // Calculate our approximation and error estimate:
      //
      result = calculate_bessel_J_as_series(x, v, &current_error);
      //
      // If the error from the current approximation is too high we'll need 
      // to loop round and try again, in this case we use the simple heuristic
      // of doubling the working precision with each loop.  More refined approaches
      // are certainly available:
      //
      digits10 *= 2;
      scoped.reset(digits10);
   }
   //
   // We now have an accurate result, but it may have too many digits,
   // so lets round the result to the requested precision now:
   //
   result.precision(saved_digits10);
   //
   // To maintain uniform precision during function return, lets
   // reset the default precision now:
   //
   scoped.reset(saved_digits10);
   return result;
}
//]

mpfr_float calculate_bessel_J_as_series_2(const mpfr_float& x, const mpfr_float& v, mpfr_float* err)
{
   mpfr_float sum        = pow(x / 2, v) / tgamma(v + 1);
   mpfr_float abs_sum    = abs(sum);
   mpfr_float multiplier = -x * x / 4;
   mpfr_float divider    = v + 1;
   mpfr_float term       = sum;
   mpfr_float eps        = boost::math::tools::epsilon<mpfr_float>();
   unsigned   m          = 1;

   while (fabs(term / abs_sum) > eps)
   {
      term *= multiplier;
      term /= m;
      term /= divider;
      ++divider;
      ++m;
      sum += term;
      abs_sum += abs(term);
   }
   if (err)
      *err = eps * fabs(abs_sum / sum);

   return sum;
}

mpfr_float Bessel_J_to_precision_2(const mpfr_float& v, const mpfr_float& x, unsigned digits10)
{
   scoped_mpfr_precision scoped(digits10);
   scoped_mpfr_precision_options scoped_opts(boost::multiprecision::variable_precision_options::preserve_target_precision);
   mpfr_float                    result;
   mpfr_float            current_error{1};
   mpfr_float            target_error{std::pow(10., -static_cast<int>(digits10))};

   while (target_error < current_error)
   {
      result = calculate_bessel_J_as_series_2(x, v, &current_error);

      digits10 *= 2;
      scoped.reset(digits10);
   }
   return result;
}

mpfr_float reduce_n_pi(const mpfr_float& arg)
{
   unsigned n = static_cast<unsigned>(arg / boost::math::constants::pi<mpfr_float>());

   scoped_mpfr_precision            scope_1(mpfr_float::thread_default_precision() * 2);
   scoped_mpfr_precision_options    scope_2(boost::multiprecision::variable_precision_options::preserve_source_precision);

   mpfr_float reduced = arg - n * boost::math::constants::pi<mpfr_float>();
   if (reduced < 0)
      reduced += boost::math::constants::pi<mpfr_float>();
   scope_1.reset();
   scope_2.reset(boost::multiprecision::variable_precision_options::preserve_target_precision);
   mpfr_float result = reduced;
   return result;
}


int main()
{
   mpfr_float::thread_default_precision(50);
   mpfr_float x{6541389046624379uLL};
   x /= 562949953421312uLL;
   mpfr_float v{2};
   mpfr_float err;
   mpfr_float J = Bessel_J_to_precision(v, x, 50);

   std::cout << std::setprecision(50) << J << std::endl;

   mpfr_float expected{"-9.31614245636402072613249153246313221710284959883647822724e-15"};
   std::cout << boost::math::relative_difference(expected, J) << std::endl;
   
   J = Bessel_J_to_precision_2(v, x, 50);
   std::cout << boost::math::relative_difference(expected, J) << std::endl;
   
   std::cout << reduce_n_pi(boost::math::float_next(boost::math::float_next(5 * boost::math::constants::pi<mpfr_float>()))) << std::endl;

   return 0;
}

