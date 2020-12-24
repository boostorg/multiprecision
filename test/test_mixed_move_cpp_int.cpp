///////////////////////////////////////////////////////////////
//  Copyright 2020 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

//
// Compare arithmetic results using fixed_int to GMP results.
//

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/multiprecision/cpp_int.hpp>
#include "test.hpp"

unsigned alloc_count = 0;
void* operator new(std::size_t count)
{
   ++alloc_count;
   return std::malloc(count);
}
void* operator new[](std::size_t count)
{
   ++alloc_count;
   return std::malloc(count);
}
void operator delete(void * p)noexcept
{
   return std::free(p);
}
void operator delete[](void* p)
{
   return std::free(p);
}


template <class From, class To>
void test_one(From val)
{
   From copy(val);
   To   target(val);
   alloc_count = 0;
   To   moved(std::move(copy));
   BOOST_CHECK_EQUAL(alloc_count, 0);
   BOOST_CHECK_EQUAL(moved, target);
   copy = val;
   moved = 0;
   BOOST_CHECK_EQUAL(copy, val);
   BOOST_CHECK_EQUAL(moved, 0);
   alloc_count = 0;
   moved = std::move(copy);
   BOOST_CHECK_EQUAL(alloc_count, 0);
   BOOST_CHECK_EQUAL(moved, target);
}

template <class From, class To>
void test()
{
   From val(10);
   From limit = pow(From(2), 1000);

   while (val < limit)
   {
      test_one<From, To>(val);
      test_one<From, To>(-val);
      val *= 100;
      val /= 3;
   }
}

int main()
{
   using namespace boost::multiprecision;
   //
   // Our "From" type has the MinBits argument as small as possible, 
   // should it be larger than the default used in cpp_int then we 
   // may get allocations causing the tests above to fail since the
   // internal cache in type From is larger than that of cpp_int and so
   // allocation may be needed even on a move.
   //
   test<number<cpp_int_backend<sizeof(limb_type) * CHAR_BIT * 2> >, cpp_int>();

   return boost::report_errors();
}
