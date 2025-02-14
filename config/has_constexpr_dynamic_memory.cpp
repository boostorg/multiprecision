//  Copyright John Maddock 2025.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <memory>


#if !defined(__cpp_constexpr_dynamic_alloc)
#  error "__cpp_constexpr_dynamic_alloc is not defined"

#elif __cpp_constexpr_dynamic_alloc < 201907
#  error "__cpp_constexpr_dynamic_alloc is defined with value < 201907"

#elif !defined(__cpp_lib_constexpr_dynamic_alloc)
#  error "__cpp_lib_constexpr_dynamic_alloc is not defined"

#elif __cpp_lib_constexpr_dynamic_alloc < 201907
#  error "__cpp_lib_constexpr_dynamic_alloc is defined with value < 201907"
#endif

int main()
{
   return 0;
}
