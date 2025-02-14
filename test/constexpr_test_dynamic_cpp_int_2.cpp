//  (C) Copyright John Maddock 2025.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>

#include "constexpr_test_dynamic_cpp_int.hpp"


int main(){
# if defined(BOOST_HAS_INT128)  // BOOST_HAS_INT128 unset is already tested
  return run_constexpr_cpp_int_tests();

# else
  return 0;
# endif
}
