///////////////////////////////////////////////////////////////////////////////
//  Copyright 2019 - 2025 John Maddock.
//  Copyright 2025 Christopher Kormanyos.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <test.hpp>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <locale>
#include <string>

int main()
{
   std::string str { };

   try
   {
      using local_dec_float_type = boost::multiprecision::cpp_dec_float_50;

      std::locale::global(std::locale("en-US"));
      local_dec_float_type d("1234.56");
      str = d.str();

      BOOST_CHECK_EQUAL(str, "1234.56");

      d = local_dec_float_type("123.456");
      str = d.str();

      BOOST_CHECK_EQUAL(str, "123.456");
   }
   catch(const std::runtime_error&){}  // No en-US locale

   BOOST_CHECK(!str.empty());
   BOOST_CHECK(str.find('.') != std::string::npos);

   return boost::report_errors();
}
