//  Copyright John Maddock 2011.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//
// This tests two things: that cpp_float_50 meets our
// conceptual requirements, and that we can instantiate
// all our distributions and special functions on this type.
//
#define BOOST_MATH_ASSERT_UNDEFINED_POLICY false
#define BOOST_MATH_INSTANTIATE_MINIMUM

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#  pragma warning(disable:4800)
#  pragma warning(disable:4512)
#  pragma warning(disable:4127)
#  pragma warning(disable:4512)
#  pragma warning(disable:4503) // decorated name length exceeded, name was truncated
#endif

#if !defined(TEST_MPF_50) && !defined(TEST_BACKEND) && !defined(TEST_MPZ) \
   && !defined(TEST_CPP_DEC_FLOAT) && !defined(TEST_MPFR_50)\
   && !defined(TEST_MPFR_6) && !defined(TEST_MPFR_15) && !defined(TEST_MPFR_17) && !defined(TEST_MPFR_30)
#  define TEST_MPF_50
#  define TEST_BACKEND
#  define TEST_MPZ
#  define TEST_MPFR_50
#  define TEST_MPFR_6
#  define TEST_MPFR_15
#  define TEST_MPFR_17
#  define TEST_MPFR_30
#  define TEST_CPP_DEC_FLOAT

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPF_50) || defined(TEST_MPZ)
#include <boost/multiprecision/gmp.hpp>
#endif
#ifdef TEST_BACKEND
#include <boost/multiprecision/concepts/mp_number_architypes.hpp>
#endif
#ifdef TEST_CPP_DEC_FLOAT
#include <boost/multiprecision/cpp_dec_float.hpp>
#endif
#if defined(TEST_MPFR_50) || defined(TEST_MPFR_6) || defined(TEST_MPFR_15) || defined(TEST_MPFR_17) || defined(TEST_MPFR_30)
#include <boost/multiprecision/mpfr.hpp>
#endif

#include <boost/math/concepts/real_type_concept.hpp>
#include "libs/math/test/compile_test/instantiate.hpp"

void foo()
{
#ifdef TEST_BACKEND
   instantiate(boost::multiprecision::concepts::mp_number_float_architype());
#endif
#ifdef TEST_MPF_50
   instantiate(boost::multiprecision::mpf_float_50());
#endif
#ifdef TEST_MPFR_50
   instantiate(boost::multiprecision::mpfr_float_50());
#endif
#ifdef TEST_MPFR_6
   instantiate(boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<6> >());
#endif
#ifdef TEST_MPFR_15
   instantiate(boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<15> >());
#endif
#ifdef TEST_MPFR_17
   instantiate(boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<17> >());
#endif
#ifdef TEST_MPFR_30
   instantiate(boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<30> >());
#endif
#ifdef TEST_CPP_DEC_FLOAT
   instantiate(boost::multiprecision::cpp_float_50());
#endif
}

int main()
{
#ifdef TEST_BACKEND
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::concepts::mp_number_float_architype>));
#endif
#ifdef TEST_MPF_50
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::mpf_float_50>));
#endif
#ifdef TEST_MPFR_50
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::mpfr_float_50>));
#endif
#ifdef TEST_MPFR_6
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<6> > >));
#endif
#ifdef TEST_MPFR_15
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<15> > >));
#endif
#ifdef TEST_MPFR_17
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<17> > >));
#endif
#ifdef TEST_MPFR_30
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<30> > >));
#endif
#ifdef TEST_MPFR_50
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::mpfr_float_50>));
#endif
#ifdef TEST_CPP_DEC_FLOAT
   BOOST_CONCEPT_ASSERT((boost::math::concepts::RealTypeConcept<boost::multiprecision::cpp_float_50>));
#endif

}
