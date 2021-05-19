///////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2021.
//  Copyright 2021 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//
// This work is based on an earlier work:
// "Algorithm 910: A Portable C++ Multiple-Precision System for Special-Function Calculations",
// in ACM TOMS, {VOL 37, ISSUE 4, (February 2011)} (C) ACM, 2011. http://doi.acm.org/10.1145/1916461.1916469

#include <boost/detail/lightweight_test.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include "test.hpp"

#include <boost/multiprecision/cpp_dec_float.hpp>

template<class T>
void test()
{
   // N[Gamma[5/2], 120]
   const T control_tgamma_2_and_half("1.32934038817913702047362561250585888709816209209179034616035584238968346344327413603121299255390849906217011771821192800");

   // N[Gamma[500/2], 120]
   const T control_tgamma_2_fifty   ("1.29314250436364309292832582080974738839793748706951226669917697084512949902204448379552716614841127978037140294127577317E490");

   const T tgamma_2_and_half = boost::math::tgamma(T(T(5)   / 2));
   const T tgamma_2_fifty    = boost::math::tgamma(T(T(500) / 2));

   BOOST_CHECK_CLOSE_FRACTION(tgamma_2_and_half, control_tgamma_2_and_half, T(std::numeric_limits<T>::epsilon() * 100));
   BOOST_CHECK_CLOSE_FRACTION(tgamma_2_fifty   , control_tgamma_2_fifty,    T(std::numeric_limits<T>::epsilon() * 100));
}

int main()
{
   using big_float_type_010 = boost::multiprecision::number<boost::multiprecision::cpp_dec_float< 10>>;
   using big_float_type_035 = boost::multiprecision::number<boost::multiprecision::cpp_dec_float< 35>>;
   using big_float_type_105 = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<105>>;

   test<big_float_type_010>();
   test<big_float_type_035>();
   test<big_float_type_105>();

   return boost::report_errors();
}
