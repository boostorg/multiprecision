///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <iostream>
#include <iomanip>

//[AOS1

/*`Generic numeric programming employs templates to use the same code for different
floating-point types and functions. Consider the area of a circle a of radius r, given by

[:['a = [pi] * r[super 2]]]

The area of a circle can be computed in generic programming using Boost.Math
for the constant [pi] as shown below:

*/

//=#include <boost/math/constants/constants.hpp>

template<typename T>
inline T area_of_a_circle(T r)
{
   using boost::math::constants::pi;
   return pi<T>() * r * r;
}

/*`
It is possible to use `area_of_a_circle()` with built-in floating-point types as
well as floating-point types from Boost.Multiprecision. In particular, consider a
system with 4-byte single-precision float, 8-byte double-precision double and also the
`cpp_dec_float_50` data type from Boost.Multiprecision with 50 decimal digits
of precision.

We can compute and print the approximate area of a circle with radius 123/100 for
`float`, `double` and `cpp_dec_float_50` with the program below.

*/

//]

//[AOS3

/*`In the next example we'll look at calling both standard library and Boost.Math functions from within generic code.
We'll also show how to cope with template arguments which are expression-templates rather than number types.*/

//]

//[JEL

/*`
In this example we'll show several implementations of the 
[@http://mathworld.wolfram.com/LambdaFunction.html Jahnke and Emden Lambda function], 
each implementation a little more sophisticated than the last.

The Jahnke-Emden Lambda function is defined by the equation:

[:['JahnkeEmden(v, z) = [Gamma](v+1) * J[sub v](z) / (z / 2)[super v]]]

If we were to implement this at double precision using Boost.Math's facilities for the Gamma and Bessel
function calls it would look like this:

*/

double JEL1(double v, double z)
{
   return boost::math::tgamma(v + 1) * boost::math::cyl_bessel_j(v, z) / std::pow(z / 2, v);
}

/*`
Calling this function as:

   std::cout << std::scientific << std::setprecision(std::numeric_limits<double>::digits10);
   std::cout << JEL1(2.5, 0.5) << std::endl;

Yields the output:

[pre 9.822663964796047e-001]

Now lets implement the function again, but this time using the multiprecision type
`cpp_dec_float_50` as the argument type:

*/   

boost::multiprecision::cpp_dec_float_50 
   JEL2(boost::multiprecision::cpp_dec_float_50 v, boost::multiprecision::cpp_dec_float_50 z)
{
   return boost::math::tgamma(v + 1) * boost::math::cyl_bessel_j(v, z) / boost::multiprecision::pow(z / 2, v);
}

/*`
The implementation is almost the same as before, but with one key difference - we can no longer call
`std::pow`, instead we must call the version inside the `boost::multiprecision` namespace.  In point of
fact, we could have omitted the namespace prefix on the call to `pow` since the right overload would 
have been found via [@http://en.wikipedia.org/wiki/Argument-dependent_name_lookup 
argument dependent lookup] in any case.

Note also that the first argument to `pow` along with the argument to `tgamma` in the above code
are actually expression templates.  The `pow` and `tgamma` functions will handle these arguments
just fine.

Here's an example of how the function may be called:

   std::cout << std::scientific << std::setprecision(std::numeric_limits<cpp_dec_float_50>::digits10);
   std::cout << JEL2(cpp_dec_float_50(2.5), cpp_dec_float_50(0.5)) << std::endl;

Which outputs:

[pre 9.82266396479604757017335009796882833995903762577173e-01]

Now that we've seen some non-template examples, lets repeat the code again, but this time as a template
that can be called either with a builtin type (`float`, `double` etc), or with a multiprecision type:

*/

template <class Float>
Float JEL3(Float v, Float z)
{
   using std::pow;
   return boost::math::tgamma(v + 1) * boost::math::cyl_bessel_j(v, z) / pow(z / 2, v);
}

/*`

Once again the code is almost the same as before, but the call to `pow` has changed yet again.
We need the call to resolve to either `std::pow` (when the argument is a builtin type), or
to `boost::multiprecision::pow` (when the argument is a multiprecision type).  We do that by
making the call unqualified so that versions of `pow` defined in the same namespace as type
`Float` are found via argument dependent lookup, while the `using std::pow` directive makes
the standard library versions visible for builtin floating point types.

Let's call the function with both `double` and multiprecision arguments:

   std::cout << std::scientific << std::setprecision(std::numeric_limits<double>::digits10);
   std::cout << JEL3(2.5, 0.5) << std::endl;
   std::cout << std::scientific << std::setprecision(std::numeric_limits<cpp_dec_float_50>::digits10);
   std::cout << JEL3(cpp_dec_float_50(2.5), cpp_dec_float_50(0.5)) << std::endl;

Which outputs:

[pre
9.822663964796047e-001
9.82266396479604757017335009796882833995903762577173e-01
]

Unfortunately there is a problem with this version: if we were to call it like this:

   boost::multiprecision::cpp_dec_float_50 v(2), z(0.5);
   JEL3(v + 0.5, z);

Then we would get a long and inscrutable error message from the compiler: the problem here is that the first
argument to `JEL3` is not a number type, but an expression template.  We could obviously add a typecast to
fix the issue:

   JEL(cpp_dec_float_50(v + 0.5), z);

However, if we want the function JEL to be truely reusable, then a better solution might be preferred.
To achieve this we can borrow some code from Boost.Math which calculates the return type of mixed-argument
functions, here's how the new code looks now:

*/

template <class Float1, class Float2>
typename boost::math::tools::promote_args<Float1, Float2>::type 
   JEL4(Float1 v, Float2 z)
{
   using std::pow;
   return boost::math::tgamma(v + 1) * boost::math::cyl_bessel_j(v, z) / pow(z / 2, v);
}

/*`

As you can see the two arguments to the function are now separate template types, and
the return type is computed using the `promote_args` metafunction from Boost.Math.

Now we can call:

   std::cout << std::scientific << std::setprecision(std::numeric_limits<cpp_dec_float_100>::digits10);
   std::cout << JEL4(cpp_dec_float_100(2) + 0.5, cpp_dec_float_100(0.5)) << std::endl;

And get 100 digits of output:

[pre 9.8226639647960475701733500979688283399590376257717309069410413822165082248153638454147004236848917775e-01]

As a bonus, we can now call the function not just with expression templates, but with other mixed types as well:
for example `float` and `double` or `int` and `double`, and the correct return type will be computed in each case.

Note that while in this case we didn't have to change the body of the function, in the general case
any function like this which creates local variables internally would have to use `promote_args`
to work out what type those variables should be, for example:

   template <class Float1, class Float2>
   typename boost::math::tools::promote_args<Float1, Float2>::type 
      JEL5(Float1 v, Float2 z)
   {
      using std::pow;
      typedef typename boost::math::tools::promote_args<Float1, Float2>::type variable_type;
      variable_type t = pow(z / 2, v);
      return boost::math::tgamma(v + 1) * boost::math::cyl_bessel_j(v, z) / t;
   }

*/

//]

//[ND1

/*`
In this example we'll add even more power to generic numeric programming using not only different
floating-point types but also function objects as template parameters. Consider
some well-known central difference rules for numerically computing the first derivative
of a function ['f[prime](x)] with ['x [isin] [real]]:

[equation floating_point_eg1]

Where the difference terms ['m[sub n]] are given by:

[equation floating_point_eg2]

and ['dx] is the step-size of the derivative.

The third formula in Equation 1 is a three-point central difference rule. It calculates
the first derivative of ['f[prime](x)] to ['O(dx[super 6])], where ['dx] is the given step-size. 
For example, if
the step-size is 0.01 this derivative calculation has about 6 decimal digits of precision - 
just about right for the 7 decimal digits of single-precision float.
Let's make a generic template subroutine using this three-point central difference
rule.  In particular:
*/

template<typename value_type, typename function_type>
   value_type derivative(const value_type x, const value_type dx, function_type func)
{
   // Compute d/dx[func(*first)] using a three-point
   // central difference rule of O(dx^6).

   const value_type dx1 = dx;
   const value_type dx2 = dx1 * 2;
   const value_type dx3 = dx1 * 3;

   const value_type m1 = (func(x + dx1) - func(x - dx1)) / 2;
   const value_type m2 = (func(x + dx2) - func(x - dx2)) / 4;
   const value_type m3 = (func(x + dx3) - func(x - dx3)) / 6;

   const value_type fifteen_m1 = 15 * m1;
   const value_type six_m2     =  6 * m2;
   const value_type ten_dx1    = 10 * dx1;

   return ((fifteen_m1 - six_m2) + m3) / ten_dx1;
}

/*`The `derivative()` template function can be used to compute the first derivative
of any function to ['O(dx[super 6])]. For example, consider the first derivative of ['sin(x)] evaluated
at ['x = [pi]/3]. In other words,

[equation floating_point_eg3]

The code below computes the derivative in Equation 3 for float, double and boost's
multiple-precision type cpp_dec_float_50.
*/

//]

//[GI1

/*`
Similar to the generic derivative example, we can calculate integrals in a similar manner:
*/

template<typename value_type, typename function_type>
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

/*`
The following sample program shows how the function can be called, we begin
by defining a function object, which when integrated should yield the Bessel J
function:
*/

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


//]

int main()
{
   using namespace boost::multiprecision;
   std::cout << std::scientific << std::setprecision(std::numeric_limits<double>::digits10);
   std::cout << JEL1(2.5, 0.5) << std::endl;
   std::cout << std::scientific << std::setprecision(std::numeric_limits<cpp_dec_float_50>::digits10);
   std::cout << JEL2(cpp_dec_float_50(2.5), cpp_dec_float_50(0.5)) << std::endl;
   std::cout << std::scientific << std::setprecision(std::numeric_limits<double>::digits10);
   std::cout << JEL3(2.5, 0.5) << std::endl;
   std::cout << std::scientific << std::setprecision(std::numeric_limits<cpp_dec_float_50>::digits10);
   std::cout << JEL3(cpp_dec_float_50(2.5), cpp_dec_float_50(0.5)) << std::endl;
   std::cout << std::scientific << std::setprecision(std::numeric_limits<cpp_dec_float_100>::digits10);
   std::cout << JEL4(cpp_dec_float_100(2) + 0.5, cpp_dec_float_100(0.5)) << std::endl;

   //[AOS2

/*=#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_dec_float.hpp>

using boost::multiprecision::cpp_dec_float_50;

int main(int, char**)
{*/
   const float r_f(float(123) / 100);
   const float a_f = area_of_a_circle(r_f);

   const double r_d(double(123) / 100);
   const double a_d = area_of_a_circle(r_d);

   const cpp_dec_float_50 r_mp(cpp_dec_float_50(123) / 100);
   const cpp_dec_float_50 a_mp = area_of_a_circle(r_mp);

   // 4.75292
   std::cout
      << std::setprecision(std::numeric_limits<float>::digits10)
      << a_f
      << std::endl;

   // 4.752915525616
   std::cout
      << std::setprecision(std::numeric_limits<double>::digits10)
      << a_d
      << std::endl;

   // 4.7529155256159981904701331745635599135018975843146
   std::cout
      << std::setprecision(std::numeric_limits<cpp_dec_float_50>::digits10)
      << a_mp
      << std::endl;
/*=}*/

   //]

   //[ND2
/*=
#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/constants/constants.hpp>


int main(int, char**)
{*/
   using boost::math::constants::pi;
   using boost::multiprecision::cpp_dec_float_50;
   //
   // We'll pass a function pointer for the function object passed to derivative,
   // the typecast is needed to select the correct overload of std::sin:
   //
   const float d_f = derivative(
      pi<float>() / 3,
      0.01F,
      static_cast<float(*)(float)>(std::sin)
   );

   const double d_d = derivative(
      pi<double>() / 3,
      0.001,
      static_cast<double(*)(double)>(std::sin)
      );
   //
   // In the cpp_dec_float_50 case, the sin function is multiply overloaded
   // to handle expression templates etc.  As a result it's hard to take it's
   // address without knowing about its implementation details.  We'll use a 
   // C++11 lambda expression to capture the call.
   // We also need a typecast on the first argument so we don't accidently pass
   // an expression template to a template function:
   //
   const cpp_dec_float_50 d_mp = derivative(
      cpp_dec_float_50(pi<cpp_dec_float_50>() / 3),
      cpp_dec_float_50(1.0E-9),
      [](const cpp_dec_float_50& x) -> cpp_dec_float_50
      {
         return sin(x);
      }
      );

   // 5.000029e-001
   std::cout
      << std::setprecision(std::numeric_limits<float>::digits10)
      << d_f
      << std::endl;

   // 4.999999999998876e-001
   std::cout
      << std::setprecision(std::numeric_limits<double>::digits10)
      << d_d
      << std::endl;

   // 4.99999999999999999999999999999999999999999999999999e-01
   std::cout
      << std::setprecision(std::numeric_limits<cpp_dec_float_50>::digits10)
      << d_mp
      << std::endl;
//=}

   /*`
   The expected value of the derivative is 0.5. This central difference rule in this
   example is ill-conditioned, meaning it suffers from slight loss of precision. With that
   in mind, the results agree with the expected value of 0.5.*/

   //]

   //[ND3

   /*`
   We can take this a step further and use our derivative function to compute
   a partial derivative.  For example if we take the incomplete gamma function
   ['P(a, z)], and take the derivative with respect to /z/ at /(2,2)/ then we
   can calculate the result as shown below, for good measure we'll compare with
   the "correct" result obtained from a call to ['gamma_p_derivative], the results
   agree to approximately 44 digits:
   */

   cpp_dec_float_50 gd = derivative(
      cpp_dec_float_50(2),
      cpp_dec_float_50(1.0E-9),
      [](const cpp_dec_float_50& x) ->cpp_dec_float_50
      {
         return boost::math::gamma_p(2, x);
      }
   );
   // 2.70670566473225383787998989944968806815263091819151e-01
   std::cout
      << std::setprecision(std::numeric_limits<cpp_dec_float_50>::digits10)
      << gd
      << std::endl;
   // 2.70670566473225383787998989944968806815253190143120e-01
   std::cout << boost::math::gamma_p_derivative(cpp_dec_float_50(2), cpp_dec_float_50(2)) << std::endl;
   //]

   //[GI2

   /* The function can now be called as follows: */
/*=int main(int, char**)
{*/
   using boost::math::constants::pi;
   typedef boost::multiprecision::cpp_dec_float_50 mp_type;

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

   //
   // Print true value for comparison:
   // 0.166369383786814073512678524315131594371033482453329
   std::cout << boost::math::cyl_bessel_j(2, mp_type(123) / 100) << std::endl;
//=}

   //]

   return 0;
}

/*

Program output:

9.822663964796047e-001
9.82266396479604757017335009796882833995903762577173e-01
9.822663964796047e-001
9.82266396479604757017335009796882833995903762577173e-01
9.8226639647960475701733500979688283399590376257717309069410413822165082248153638454147004236848917775e-01
4.752916e+000
4.752915525615998e+000
4.75291552561599819047013317456355991350189758431460e+00
5.000029e-001
4.999999999998876e-001
4.99999999999999999999999999999999999999999999999999e-01
2.70670566473225383787998989944968806815263091819151e-01
2.70670566473225383787998989944968806815253190143120e-01
*/