///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/special_functions/bessel.hpp>
#include <iostream>
#include <iomanip>

//[JEL

/*`
In this example we'll show several implementations of the 
[@http://mathworld.wolfram.com/LambdaFunction.html Jahnke and Emden Lambda function], 
each implementation a little more sophisticated than the last.

The Jahnke-Emden Lambda function is defined by the equation:

JahnkeEmden(v, z) = Gamma(v+1) * J[v](z) / (z / 2)^v

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
   return 0;
}

/*

Program output:

9.822663964796047e-001
9.82266396479604757017335009796882833995903762577173e-01
9.822663964796047e-001
9.82266396479604757017335009796882833995903762577173e-01
9.8226639647960475701733500979688283399590376257717309069410413822165082248153638454147004236848917775e-01
*/