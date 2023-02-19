///////////////////////////////////////////////////////////////////////////////
//  Copyright 2023 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_TEST_FORMAT_HPP
#define BOOST_MP_TEST_FORMAT_HPP

#include "test.hpp"
#include <format>

#ifdef BOOST_NO_CXX20_HDR_FORMAT
#error "No support for std::format!!"
#endif

#ifdef TEST_WIDE_CHAR
#define C(x) BOOST_JOIN(L, x)

// To keep our testing framework happy:
std::ostream& operator<<(std::ostream& os, const std::wstring& s)
{
   std::string ns(s.begin(), s.end());
   return os << ns;
}

using string_type = std::wstring;

#else
#define C(x) x
using string_type = std::string;
#endif

template <class Number>
void test_integer()
{
   Number val(3);
   Number mval(-3);
   
   BOOST_CHECK_EQUAL(std::format(C("{}"), val), string_type(C("3")));
   BOOST_CHECK_EQUAL(std::format(C("{:<}"), val), string_type(C("3")));
   BOOST_CHECK_EQUAL(std::format(C("{:^}"), val), string_type(C("3")));
   BOOST_CHECK_EQUAL(std::format(C("{:>}"), val), string_type(C("3")));
   BOOST_CHECK_EQUAL(std::format(C("{:<10}"), val), string_type(C("3         ")));
   BOOST_CHECK_EQUAL(std::format(C("{:^8}"), val), string_type(C("   3    ")));
   BOOST_CHECK_EQUAL(std::format(C("{:>20}"), val), string_type(C("                   3")));
   BOOST_CHECK_EQUAL(std::format(C("{: <10}"), val), string_type(C("3         ")));
   BOOST_CHECK_EQUAL(std::format(C("{: ^8}"), val), string_type(C("   3    ")));
   BOOST_CHECK_EQUAL(std::format(C("{: >20}"), val), string_type(C("                   3")));
   BOOST_CHECK_EQUAL(std::format(C("{:#<10}"), val), string_type(C("3#########")));
   BOOST_CHECK_EQUAL(std::format(C("{:#^8}"), val), string_type(C("###3####")));
   BOOST_CHECK_EQUAL(std::format(C("{:#>20}"), val), string_type(C("###################3")));
   
   BOOST_CHECK_EQUAL(std::format(C("{:+}"), val), string_type(C("+3")));
   BOOST_CHECK_EQUAL(std::format(C("{:-}"), val), string_type(C("3")));
   BOOST_CHECK_EQUAL(std::format(C("{: }"), val), string_type(C(" 3")));
   if constexpr (std::numeric_limits<Number>::is_signed)
   {
      BOOST_CHECK_EQUAL(std::format(C("{:+}"), mval), string_type(C("-3")));
      BOOST_CHECK_EQUAL(std::format(C("{:-}"), mval), string_type(C("-3")));
      BOOST_CHECK_EQUAL(std::format(C("{: }"), mval), string_type(C("-3")));
   }
   BOOST_CHECK_EQUAL(std::format(C("{:+05}"), val), string_type(C("+0003")));
   BOOST_CHECK_EQUAL(std::format(C("{:-05}"), val), string_type(C("00003")));
   BOOST_CHECK_EQUAL(std::format(C("{: 05}"), val), string_type(C(" 0003")));
   if constexpr (std::numeric_limits<Number>::is_signed)
   {
      BOOST_CHECK_EQUAL(std::format(C("{:+05}"), mval), string_type(C("-0003")));
      BOOST_CHECK_EQUAL(std::format(C("{:-05}"), mval), string_type(C("-0003")));
      BOOST_CHECK_EQUAL(std::format(C("{: 05}"), mval), string_type(C("-0003")));
   }

   BOOST_CHECK_EQUAL(std::format(C("{:.10}"), val), string_type(C("3"))); // precision is parsed by inetegers, but has no effect.
   //BOOST_CHECK_EQUAL(std::format(C("{:L}"), val), string_type(C("3")));

   val = 324;
   BOOST_CHECK_EQUAL(std::format(C("{:o}"), val), string_type(C("504")));
   BOOST_CHECK_EQUAL(std::format(C("{:#o}"), val), string_type(C("0504")));
   BOOST_CHECK_EQUAL(std::format(C("{:x}"), val), string_type(C("144")));
   BOOST_CHECK_EQUAL(std::format(C("{:#x}"), val), string_type(C("0x144")));
   BOOST_CHECK_EQUAL(std::format(C("{:#X}"), val), string_type(C("0X144")));
   BOOST_CHECK_EQUAL(std::format(C("{:d}"), val), string_type(C("324")));
   BOOST_CHECK_EQUAL(std::format(C("{:#d}"), val), string_type(C("324")));
   BOOST_CHECK_EQUAL(std::format(C("{:b}"), val), string_type(C("101000100")));
   BOOST_CHECK_EQUAL(std::format(C("{:#b}"), val), string_type(C("0b101000100")));
   BOOST_CHECK_EQUAL(std::format(C("{:#B}"), val), string_type(C("0B101000100")));
   val <<= 100;
   BOOST_CHECK_EQUAL(std::format(C("{:#B}"), val), string_type(C("0B1010001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")));

   try
   {
      std::locale loc("en");
      std::locale cloc("C");
      BOOST_CHECK_EQUAL(std::format(cloc, C("{:L}"), val), string_type(C("410718794473946326084931838541824")));
      BOOST_CHECK_EQUAL(std::format(loc, C("{:L}"), val), string_type(C("410,718,794,473,946,326,084,931,838,541,824")));
      BOOST_CHECK_EQUAL(std::format(loc, C("{:Ld}"), val), string_type(C("410,718,794,473,946,326,084,931,838,541,824")));
      BOOST_CHECK_EQUAL(std::format(loc, C("{:Lo}"), val), string_type(C("1,210,000,000,000,000,000,000,000,000,000,000,000")));
      BOOST_CHECK_EQUAL(std::format(loc, C("{:#Lo}"), val), string_type(C("01,210,000,000,000,000,000,000,000,000,000,000,000")));
      BOOST_CHECK_EQUAL(std::format(loc, C("{:Lx}"), val), string_type(C("1,440,000,000,000,000,000,000,000,000")));
      BOOST_CHECK_EQUAL(std::format(loc, C("{:#Lx}"), val), string_type(C("0x1,440,000,000,000,000,000,000,000,000")));
      BOOST_CHECK_EQUAL(std::format(loc, C("{:Lb}"), val), string_type(C("1,010,001,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000")));
      BOOST_CHECK_EQUAL(std::format(loc, C("{:#Lb}"), val), string_type(C("0b1,010,001,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000")));
   }
   catch (...)
   {}


   val = 32;
   BOOST_CHECK_EQUAL(std::format(C("{:c}"), val), string_type(C(" ")));
   val = 100000;
   BOOST_CHECK_THROW(std::format(C("{:c}"), val), std::format_error);
}

#endif
