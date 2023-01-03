///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 - 2023 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DOUBLE_QUAD_FP_ARITHMETIC_2023_01_02_HPP
#define BOOST_MP_CPP_DOUBLE_QUAD_FP_ARITHMETIC_2023_01_02_HPP

#include <tuple>
#include <utility>

#include <boost/config.hpp>
#if defined(BOOST_MATH_USE_FLOAT128)
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/number.hpp>

namespace boost { namespace multiprecision { namespace backends { namespace detail {

template <class T>
struct is_floating_point_or_float128
{
   static constexpr bool value =
   (
         std::is_floating_point<T>::value
      #if defined(BOOST_MATH_USE_FLOAT128)
      || std::is_same<typename std::decay<T>::type, boost::multiprecision::float128>::value
      #endif
   );
};

template <typename FloatingPointType>
struct exact_arithmetic
{
   // The exact_arithmetic<> struct implements extended precision
   // techniques that are used in cpp_double_fp_backend and cpp_quad_float.

   static_assert(detail::is_floating_point_or_float128<FloatingPointType>::value,
                 "Error: exact_arithmetic<> invoked with unknown floating-point type");

   using float_type  = FloatingPointType;
   using float_pair  = std::pair<float_type, float_type>;
   using float_tuple = std::tuple<float_type, float_type, float_type, float_type>;

   static BOOST_MP_CXX14_CONSTEXPR float_pair split(const float_type& a)
   {
      // Split a floating point number in two (high and low) parts approximating the
      // upper-half and lower-half bits of the float

      static_assert(detail::is_floating_point_or_float128<FloatingPointType>::value,
                    "Error: exact_arithmetic<>::split invoked with unknown floating-point type");

      // TODO Replace bit shifts with constexpr funcs or ldexp for better compaitibility
      constexpr int MantissaBits = std::numeric_limits<float_type>::digits;
      constexpr int SplitBits    = MantissaBits / 2 + 1;

      // Check if the integer is wide enough to hold the Splitter.
      static_assert(std::numeric_limits<uintmax_t>::digits > SplitBits,
                    "Inadequate integer width for binary shifting needed in split(), try using ldexp instead");

      // If the above line gives an compilation error, replace the
      // line below it with the commented line

      constexpr float_type Splitter       = FloatingPointType((uintmax_t(1) << SplitBits) + 1);
      const     float_type SplitThreshold = (std::numeric_limits<float_type>::max)() / (Splitter * 2);

      float_type hi { };
      float_type lo { };

      // Handle if multiplication with the splitter would cause overflow
      if (a > SplitThreshold || a < -SplitThreshold)
      {
         constexpr float_type Normalizer = float_type(1ULL << (SplitBits + 1));

         const float_type a_ = a / Normalizer;

         const float_type temp = Splitter * a_;

         hi   = temp - (temp - a_);
         lo   = a_ - hi;

         hi *= Normalizer;
         lo *= Normalizer;
      }
      else
      {
         const float_type temp = Splitter * a;

         hi   = temp - (temp - a);
         lo   = a - hi;
      }

      return std::make_pair(hi, lo);
   }

   static BOOST_MP_CXX14_CONSTEXPR float_pair fast_sum(const float_type& a, const float_type& b)
   {
      // Exact addition of two floating point numbers, given |a| > |b|
      const float_type a_plus_b = a + b;

      return float_pair(a_plus_b, b - (a_plus_b - a));
   }

   static BOOST_MP_CXX14_CONSTEXPR float_pair sum(const float_type& a, const float_type& b)
   {
      // Exact addition of two floating point numbers
      const float_type a_plus_b = a + b;
      const float_type v        = a_plus_b - a;

      return float_pair(a_plus_b, (a - (a_plus_b - v)) + (b - v));
   }

   static BOOST_MP_CXX14_CONSTEXPR void three_sum(float_type& a, float_type& b, float_type& c)
   {
      using std::get;
      using std::tie;

      std::tuple<float_type, float_type, float_type> t;

      tie(get<0>(t), get<1>(t)) = sum(a, b);
      tie(a, get<2>(t))         = sum(c, get<0>(t));
      tie(b, c)                 = sum(get<1>(t), get<2>(t));
   }

   static BOOST_MP_CXX14_CONSTEXPR void sum(float_pair& p, float_type& e)
   {
      using std::tie;

      float_pair t;
      float_type t_;

      t                = sum(p.first, p.second);
      tie(p.first, t_) = sum(e, t.first);
      tie(p.second, e) = sum(t.second, t_);
   }

   static BOOST_MP_CXX14_CONSTEXPR float_tuple four_sum(float_type a, float_type b, float_type c, float_type d)
   {
      float_tuple out = std::make_tuple(a, b, c, d);
      normalize(out);
      return out;
   }

   static BOOST_MP_CXX14_CONSTEXPR float_pair difference(const float_type& a, const float_type& b)
   {
      // Exact subtraction of two floating point numbers
      const float_type a_minus_b = a - b;
      const float_type v         = a_minus_b - a;

      return float_pair(a_minus_b, (a - (a_minus_b - v)) - (b + v));
   }

   static BOOST_MP_CXX14_CONSTEXPR float_pair product(const float_type& a, const float_type& b)
   {
      // Exact product of two floating point numbers
      const float_pair a_split = split(a);
      const float_pair b_split = split(b);

      const volatile float_type pf = a * b;

      return
        std::make_pair
        (
          const_cast<const float_type&>(pf),
            (
                ((a_split.first  * b_split.first) - const_cast<const float_type&>(pf))
              +  (a_split.first  * b_split.second)
              +  (a_split.second * b_split.first)
            )
          +
            (a_split.second * b_split.second)
        );
   }

   static BOOST_MP_CXX14_CONSTEXPR void normalize(float_pair& p, bool fast = true)
   {
      // Converts a pair of floats to standard form.

      p = (fast ? fast_sum(p.first, p.second) : sum(p.first, p.second));
   }

   static BOOST_MP_CXX14_CONSTEXPR void normalize(float_tuple& t)
   {
      using std::get;
      using std::tie;

      float_tuple s(float_type(0.0F), float_type(0.0F), float_type(0.0F), float_type(0.0F));

      tie(get<0>(s), get<3>(t)) = fast_sum(get<2>(t), get<3>(t));
      tie(get<0>(s), get<2>(t)) = fast_sum(get<1>(t), get<0>(s));
      tie(get<0>(t), get<1>(t)) = fast_sum(get<0>(t), get<0>(s));

      tie(get<0>(s), get<1>(s)) = std::make_tuple(get<0>(t), get<1>(t));

      if (get<1>(s) != 0)
      {
         tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<2>(t));

         (get<2>(s) != 0) ? tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), get<3>(t))
                          : tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));
      }
      else
      {
         tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<2>(t));

         (get<1>(s) != 0) ? tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t))
                          : tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<3>(t));
      }

      t = s;
   }

   static BOOST_MP_CXX14_CONSTEXPR void normalize(float_tuple& t, float_type e)
   {
      using std::get;
      using std::tie;

      float_tuple s(float_type(0.0F), float_type(0.0F), float_type(0.0F), float_type(0.0F));

      tie(get<0>(s), e)         = fast_sum(get<3>(t), e);
      tie(get<0>(s), get<3>(t)) = fast_sum(get<2>(t), get<0>(s));
      tie(get<0>(s), get<2>(t)) = fast_sum(get<1>(t), get<0>(s));
      tie(get<0>(t), get<1>(t)) = fast_sum(get<0>(t), get<0>(s));

      tie(get<0>(s), get<1>(s)) = std::make_tuple(get<0>(t), get<1>(t));

      if (get<1>(s) != 0)
      {
         tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<2>(t));

         if (get<2>(s) != 0)
         {
            tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), get<3>(t));

            if(get<3>(s) != 0)
            {
              get<3>(s) += e;
            }
            else
            {
              tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e);
            }
         }
         else
         {
            tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));

            (get<2>(s) != 0) ? tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e)
                             : tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e);
         }
      }
      else
      {
         tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<2>(t));

         if (get<1>(s) != 0)
         {
            tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));

            (get<2>(s) != 0) ? tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e)
                             : tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e);
         }
         else
         {
            tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<3>(t));

            (get<1>(s) != 0) ? tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e)
                             : tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), e);
         }
      }

      t = s;
   }
};

} } } } // namespace boost::multiprecision::backends::detail

#endif // BOOST_MP_CPP_DOUBLE_QUAD_FP_ARITHMETIC_2023_01_02_HPP
