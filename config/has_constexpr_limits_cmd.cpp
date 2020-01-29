//  Copyright John Maddock 2019.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This determines if std::numeric_limits can be constexpr for the compiler/configuration.

#ifndef __GNUC__
#  error "Compiler is not GCC."
#endif
#if __GNUC__ < 9
#  error "Older GCC versions don't support -fconstexpr-ops-limit (aka -fconstexpr-loop-limit=n) ."
#endif

int main()
{
   return 0;
}
