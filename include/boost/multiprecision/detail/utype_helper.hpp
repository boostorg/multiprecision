///////////////////////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock.
//  Copyright Christopher Kormanyos 2013. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_UTYPE_HELPER_HPP
#define BOOST_MP_UTYPE_HELPER_HPP

#include <limits>
#include <cstdint>

namespace boost {
namespace multiprecision {
namespace detail {
template <const unsigned>
struct utype_helper
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<0U>
{
   typedef boost::uint8_t exact;
};
template <>
struct utype_helper<1U>
{
   typedef boost::uint8_t exact;
};
template <>
struct utype_helper<2U>
{
   typedef boost::uint8_t exact;
};
template <>
struct utype_helper<3U>
{
   typedef boost::uint8_t exact;
};
template <>
struct utype_helper<4U>
{
   typedef boost::uint8_t exact;
};
template <>
struct utype_helper<5U>
{
   typedef boost::uint8_t exact;
};
template <>
struct utype_helper<6U>
{
   typedef boost::uint8_t exact;
};
template <>
struct utype_helper<7U>
{
   typedef boost::uint8_t exact;
};
template <>
struct utype_helper<8U>
{
   typedef boost::uint8_t exact;
};

template <>
struct utype_helper<9U>
{
   typedef std::uint16_t exact;
};
template <>
struct utype_helper<10U>
{
   typedef std::uint16_t exact;
};
template <>
struct utype_helper<11U>
{
   typedef std::uint16_t exact;
};
template <>
struct utype_helper<12U>
{
   typedef std::uint16_t exact;
};
template <>
struct utype_helper<13U>
{
   typedef std::uint16_t exact;
};
template <>
struct utype_helper<14U>
{
   typedef std::uint16_t exact;
};
template <>
struct utype_helper<15U>
{
   typedef std::uint16_t exact;
};
template <>
struct utype_helper<16U>
{
   typedef std::uint16_t exact;
};

template <>
struct utype_helper<17U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<18U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<19U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<20U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<21U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<22U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<23U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<24U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<25U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<26U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<27U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<28U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<29U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<30U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<31U>
{
   typedef std::uint32_t exact;
};
template <>
struct utype_helper<32U>
{
   typedef std::uint32_t exact;
};

template <>
struct utype_helper<33U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<34U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<35U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<36U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<37U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<38U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<39U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<40U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<41U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<42U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<43U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<44U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<45U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<46U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<47U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<48U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<49U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<50U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<51U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<52U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<53U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<54U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<55U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<56U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<57U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<58U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<59U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<60U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<61U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<62U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<63U>
{
   typedef std::uint64_t exact;
};
template <>
struct utype_helper<64U>
{
   typedef std::uint64_t exact;
};

template <class unsigned_type>
int utype_prior(unsigned_type ui)
{
   // TBD: Implement a templated binary search for this.
   int priority_bit;

   unsigned_type priority_mask = unsigned_type(unsigned_type(1U) << (std::numeric_limits<unsigned_type>::digits - 1));

   for (priority_bit = std::numeric_limits<unsigned_type>::digits - 1; priority_bit >= 0; --priority_bit)
   {
      if (unsigned_type(priority_mask & ui) != unsigned_type(0U))
      {
         break;
      }

      priority_mask >>= 1;
   }

   return priority_bit;
}

}}} // namespace boost::multiprecision::detail

#endif // BOOST_MP_UTYPE_HELPER_HPP
