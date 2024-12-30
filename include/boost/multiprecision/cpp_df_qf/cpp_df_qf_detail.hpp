///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 - 2024 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DF_QF_DETAIL_2023_01_02_HPP
#define BOOST_MP_CPP_DF_QF_DETAIL_2023_01_02_HPP

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath.hpp>

#include <utility>

namespace boost { namespace multiprecision { namespace backends { namespace cpp_df_qf_detail {

template <class FloatingPointType>
struct is_floating_point_or_float128
{
   static constexpr auto value =    std::is_same<FloatingPointType, float>::value
                                 || std::is_same<FloatingPointType, double>::value
                                 || std::is_same<FloatingPointType, long double>::value
#if defined(BOOST_HAS_FLOAT128)
                                 || std::is_same<FloatingPointType, ::boost::float128_type>::value
#endif
                                 ;
};

template <typename FloatType>
struct split_maker
{
private:
   using float_type = FloatType;

public:
   static constexpr int
      n_shl
      {
         static_cast<int>((ccmath::numeric_limits<float_type>::digits + 1) / 2)
      };

      static_assert(n_shl < std::numeric_limits<std::uint64_t>::digits,
                    "Error: Left-shift amount for split does not fin in std::uint64_t");

   static constexpr float_type
      value
      {
         static_cast<float_type>
         (
            std::uint64_t
            {
                 UINT64_C(1)
               + std::uint64_t { UINT64_C(1) << static_cast<unsigned>(n_shl) }
            }
         )
      };
};

template <typename FloatingPointType>
struct exact_arithmetic
{
   // The exact_arithmetic<> struct implements extended precision
   // techniques that are used in cpp_double_fp_backend and cpp_quad_float.

   static_assert(is_floating_point_or_float128<FloatingPointType>::value,
                 "Error: exact_arithmetic<> invoked with unknown floating-point type");

   using float_type  = FloatingPointType;
   using float_pair  = std::pair<float_type, float_type>;

   static constexpr auto two_sum(const float_type a, const float_type b) -> float_pair
   {
     const float_type hi { a + b };
     const float_type a1 { hi - b };

     return { hi, float_type { (a - a1) + (b - float_type { hi - a1 }) } };
   }

   static constexpr auto two_diff(const float_type a, const float_type b) -> float_pair
   {
     const float_type hi { a - b };
     const float_type a1 { hi + b };

     return { hi, float_type { (a - a1) - (b + float_type { hi - a1 }) } };
   }

   static constexpr auto two_hilo_sum(const float_type a, const float_type b) -> float_pair
   {
      const float_type hi { a + b };

      return { hi, float_type { b - (hi - a) } };
   }

   static constexpr auto normalize(float_type a, float_type b) -> float_pair
   {
      const float_type u { a + b };

      return
      {
         u,
         float_type { a - u } + b
      };
   }
};

} } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail

#endif // BOOST_MP_CPP_DF_QF_DETAIL_2023_01_02_HPP
