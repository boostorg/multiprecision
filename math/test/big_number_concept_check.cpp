
//  Copyright John Maddock 2011.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//
// This tests two things: that e_float meets our
// conceptual requirements, and that we can instantiate
// all our distributions and special functions on this type.
//
#define BOOST_MATH_ASSERT_UNDEFINED_POLICY false
#define BOOST_MATH_INSTANTIATE_MINIMUM

#ifdef _MSC_VER
#  pragma warning(disable:4800)
#  pragma warning(disable:4512)
#  pragma warning(disable:4127)
#  pragma warning(disable:4512)
#  pragma warning(disable:4503) // decorated name length exceeded, name was truncated
#endif

#if !defined(TEST_MPF50) && !defined(TEST_MPF) && !defined(TEST_BACKEND) && !defined(TEST_MPZ) && !defined(TEST_E_FLOAT) && !defined(TEST_MPFR) && !defined(TEST_MPFR_50)
#  define TEST_MPF50
#  define TEST_MPF
#  define TEST_BACKEND
#  define TEST_MPZ
#  define TEST_MPFR
#  define TEST_MPFR_50
#  define TEST_E_FLOAT

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPF50) || defined(TEST_MPF) || defined(TEST_MPZ)
#include <boost/math/big_number/gmp.hpp>
#endif
#ifdef TEST_BACKEND
#include <boost/math/concepts/big_number_architypes.hpp>
#endif
#ifdef TEST_E_FLOAT
#include <boost/math/big_number/e_float.hpp>
#endif
#if defined(TEST_MPFR) || defined(TEST_MPFR_50)
#include <boost/math/big_number/mpfr.hpp>
#endif

#include <boost/math/concepts/real_type_concept.hpp>
#include "libs/math/test/compile_test/instantiate.hpp"

void a()
{
   using namespace boost;
   using namespace boost::math;
   using namespace boost::math::concepts;

   function_requires<DistributionConcept<bernoulli_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<beta_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<binomial_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<cauchy_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<chi_squared_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<exponential_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<extreme_value_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<fisher_f_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<gamma_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<geometric_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<hypergeometric_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<inverse_chi_squared_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<inverse_gamma_distribution<boost::math::mpfr_real_50> > >();
}

void b()
{
   using namespace boost;
   using namespace boost::math;
   using namespace boost::math::concepts;

   function_requires<DistributionConcept<inverse_gaussian_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<laplace_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<logistic_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<lognormal_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<negative_binomial_distribution<boost::math::mpfr_real_50> > >();
}

void c()
{
   using namespace boost;
   using namespace boost::math;
   using namespace boost::math::concepts;

   function_requires<DistributionConcept<non_central_chi_squared_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<non_central_beta_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<non_central_f_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<non_central_t_distribution<boost::math::mpfr_real_50> > >();
}

void d()
{
   using namespace boost;
   using namespace boost::math;
   using namespace boost::math::concepts;

   function_requires<DistributionConcept<normal_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<pareto_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<poisson_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<rayleigh_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<students_t_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<triangular_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<uniform_distribution<boost::math::mpfr_real_50> > >();
   function_requires<DistributionConcept<weibull_distribution<boost::math::mpfr_real_50> > >();
}

void foo()
{
#ifdef TEST_BACKEND
   instantiate(boost::math::big_number_real_architype());
#endif
#ifdef TEST_MPF_50
   instantiate(boost::math::mpf_real_50());
#endif
#ifdef TEST_MPF
   instantiate(boost::math::mpf_real());
#endif
#ifdef TEST_MPFR_50
   instantiate(boost::math::mpfr_real_50());
#endif
#ifdef TEST_MPFR
   instantiate(boost::math::mpfr_real());
#endif
#ifdef TEST_E_FLOAT
   instantiate(boost::math::e_float());
#endif
}

int main()
{
#ifdef TEST_BACKEND
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::math::concepts::big_number_real_architype>));
#endif
#ifdef TEST_MPF_50
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::math::mpf_real_50>));
#endif
#ifdef TEST_MPF
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::math::mpf_real>));
#endif
#ifdef TEST_MPFR_50
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::math::mpfr_real_50>));
#endif
#ifdef TEST_MPFR
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::math::mpfr_real>));
#endif
#ifdef TEST_E_FLOAT
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::math::e_float>));
#endif

}
