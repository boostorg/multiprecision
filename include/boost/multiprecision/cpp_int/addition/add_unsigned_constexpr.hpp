///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_MP_ADD_UNSIGNED_CONSTEXPR_HPP
#define BOOST_MP_ADD_UNSIGNED_CONSTEXPR_HPP

namespace boost { namespace multiprecision { namespace backends {

//
// This is the key addition routine where all the argument types are non-trivial cpp_int's:
//
template <class CppInt1, class CppInt2, class CppInt3>
inline BOOST_MP_CXX14_CONSTEXPR void add_unsigned_constexpr(CppInt1& result, const CppInt2& a, const CppInt3& b) BOOST_MP_NOEXCEPT_IF(is_non_throwing_cpp_int<CppInt1>::value)
{
   using ::boost::multiprecision::std_constexpr::swap;

   // Nothing fancy, just let uintmax_t take the strain:
   double_limb_type carry = 0;
   unsigned         m(0), x(0);
   unsigned         as = a.size();
   unsigned         bs = b.size();
   minmax(as, bs, m, x);
   if (x == 1)
   {
      bool s = a.sign();
      result = static_cast<double_limb_type>(*a.limbs()) + static_cast<double_limb_type>(*b.limbs());
      result.sign(s);
      return;
   }
   result.resize(x, x);
   typename CppInt2::const_limb_pointer pa = a.limbs();
   typename CppInt3::const_limb_pointer pb = b.limbs();
   typename CppInt1::limb_pointer       pr = result.limbs();
   typename CppInt1::limb_pointer       pr_end = pr + m;

   if (as < bs)
      swap(pa, pb);

   // First where a and b overlap:
   while (pr != pr_end)
   {
      carry += static_cast<double_limb_type>(*pa) + static_cast<double_limb_type>(*pb);
#ifdef __MSVC_RUNTIME_CHECKS
      * pr = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
#else
      * pr = static_cast<limb_type>(carry);
#endif
      carry >>= CppInt1::limb_bits;
      ++pr, ++pa, ++pb;
   }
   pr_end += x - m;
   // Now where only a has digits:
   while (pr != pr_end)
   {
      if (!carry)
      {
         if (pa != pr)
            std_constexpr::copy(pa, pa + (pr_end - pr), pr);
         break;
      }
      carry += static_cast<double_limb_type>(*pa);
#ifdef __MSVC_RUNTIME_CHECKS
      * pr = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
#else
      * pr = static_cast<limb_type>(carry);
#endif
      carry >>= CppInt1::limb_bits;
      ++pr, ++pa;
   }
   if (carry)
   {
      // We overflowed, need to add one more limb:
      result.resize(x + 1, x + 1);
      if (result.size() > x)
         result.limbs()[x] = static_cast<limb_type>(carry);
   }
   result.normalize();
   result.sign(a.sign());
}


} } }  // namespaces


#endif


