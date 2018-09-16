///////////////////////////////////////////////////////////////////////////////
//  Copyright 2018 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>

#if !BOOST_WORKAROUND(BOOST_GCC, < 70000)  // GCC prior to 7.x has serious problems with this.

template <class T>
constexpr T make_rvalue_ref(T t) { return t; }

template <class T>
struct has_complement_operator : public boost::mpl::true_ {};

template <unsigned MinBits, unsigned MaxBits>
struct has_complement_operator<boost::multiprecision::number<boost::multiprecision::cpp_int_backend<MinBits, MaxBits, boost::multiprecision::signed_magnitude, boost::multiprecision::checked> > > : public boost::mpl::false_ {};

template <class T>
struct has_negate_operator : public boost::mpl::true_ {};

template <unsigned MinBits, unsigned MaxBits>
struct has_negate_operator<boost::multiprecision::number<boost::multiprecision::cpp_int_backend<MinBits, MaxBits, boost::multiprecision::unsigned_magnitude, boost::multiprecision::checked> > > : public boost::mpl::false_ {};
template <unsigned MinBits, unsigned MaxBits>
struct has_negate_operator<boost::multiprecision::number<boost::multiprecision::cpp_int_backend<MinBits, MaxBits, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked> > > : public boost::mpl::false_ {};

template <class T>
constexpr T test_complement(T i, const boost::mpl::true_&)
{
   i = ~i;
   i &= ~make_rvalue_ref(i);
   return i;
}
template <class T>
constexpr T test_complement(T i, const boost::mpl::false_&) { return i; }

template <class T>
constexpr T test_negate(T i, const boost::mpl::true_&)
{
   i = -i;
   return -i;
}
template <class T>
constexpr T test_negate(T i, const boost::mpl::false_&) { return i; }

template <class T, class U>
constexpr T test_mixed(T, U j)
{
   T i(j);

   i |= j;
   i = i | j;
   i = j | i;
   i |= make_rvalue_ref(i) | j;
   i |= i | make_rvalue_ref(j);
   i |= make_rvalue_ref(i) | make_rvalue_ref(j);
   i |= make_rvalue_ref(j) | i;
   i |= j | make_rvalue_ref(i);
   i |= make_rvalue_ref(j) | make_rvalue_ref(i);
   
   i = j;
   //j = static_cast<U>(i);

   i &= j;
   i = i & j;
   i = j & i;
   i |= make_rvalue_ref(i) & j;
   i |= i & make_rvalue_ref(j);
   i |= make_rvalue_ref(i) & make_rvalue_ref(j);
   i |= make_rvalue_ref(j) & i;
   i |= j & make_rvalue_ref(i);
   i |= make_rvalue_ref(j) & make_rvalue_ref(i);

   i ^= j;
   i = i ^ j;
   i |= make_rvalue_ref(i) ^ j;
   i |= i ^ make_rvalue_ref(j);
   i |= make_rvalue_ref(i) ^ make_rvalue_ref(j);
   i = j ^ i;
   i |= make_rvalue_ref(j) ^ i;
   i |= j ^ make_rvalue_ref(i);
   i |= make_rvalue_ref(j) ^ make_rvalue_ref(i);

   i += j;
   i = i + j;
   i += make_rvalue_ref(i) + j;
   i += i + make_rvalue_ref(j);
   i += make_rvalue_ref(i) + make_rvalue_ref(j);
   i += make_rvalue_ref(j) + i;
   i += j + make_rvalue_ref(i);
   i += make_rvalue_ref(j) + make_rvalue_ref(i);

   i -= j;
   i = i - j;
   i += make_rvalue_ref(i) - j;
   i += i - make_rvalue_ref(j);
   i += make_rvalue_ref(i) - make_rvalue_ref(j);
   i = 2;
   i = make_rvalue_ref(j) - i;
   i = j - make_rvalue_ref(i);
   i = make_rvalue_ref(j) - make_rvalue_ref(i);
   
   i *= j;
   i = i * j;
   i += make_rvalue_ref(i) * j;
   i += i * make_rvalue_ref(j);
   i += make_rvalue_ref(i) * make_rvalue_ref(j);
   i = j * i;
   i += make_rvalue_ref(j) * i;
   i += j * make_rvalue_ref(i);
   i += make_rvalue_ref(j) * make_rvalue_ref(i);

   i /= j;
   i = i / j;
   i += make_rvalue_ref(i) / j;
   i += i / make_rvalue_ref(j);
   i += make_rvalue_ref(i) / make_rvalue_ref(j);
   i = 2;
   i = j / i;
   i += make_rvalue_ref(j) / i;
   i += j / make_rvalue_ref(i);
   i += make_rvalue_ref(j) / make_rvalue_ref(i);

   i %= j;
   i = i % j;
   i += make_rvalue_ref(i) % j;
   i += i % make_rvalue_ref(j);
   i += make_rvalue_ref(i) % make_rvalue_ref(j);
   i = 2;
   i = j % i;
   i += make_rvalue_ref(j) % i;
   i += j % make_rvalue_ref(i);
   i += make_rvalue_ref(j) % make_rvalue_ref(i);

   bool b = i == j;
   b &= i < j;
   b |= i > j;
   b |= i >= j;
   b &= i <= j;
   b &= i != j;
   b &= j < i;
   b |= j > i;
   b |= j >= i;
   b &= j <= i;
   b &= j != i;
   b = i.compare(j);

   return i;
}

template <class T>
constexpr T get_constexpr()
{
   T i = 2u;
   T j = 3u;

   i |= j;
   i = i | j;
   i |= make_rvalue_ref(i) | j;
   i |= i | make_rvalue_ref(j);
   i |= make_rvalue_ref(i) | make_rvalue_ref(j);

   i &= j;
   i = i & j;
   i |= make_rvalue_ref(i) & j;
   i |= i & make_rvalue_ref(j);
   i |= make_rvalue_ref(i) & make_rvalue_ref(j);

   i ^= j;
   i = i ^ j;
   i |= make_rvalue_ref(i) ^ j;
   i |= i ^ make_rvalue_ref(j);
   i |= make_rvalue_ref(i) ^ make_rvalue_ref(j);

   i &= test_complement(i, has_complement_operator<T>());

   i = +i;
   i = test_negate(i, has_negate_operator<T>());

   i <<= 2;
   i >>= 2;
   i = j << 2;
   i = j >> 2;

   ++i;
   i++;
   --i;
   i--;

   i += j;
   i = i + j;
   i += make_rvalue_ref(i) + j;
   i += i + make_rvalue_ref(j);
   i += make_rvalue_ref(i) + make_rvalue_ref(j);
   i -= j;
   i = i - j;
   i += make_rvalue_ref(i) - j;
   i += i - make_rvalue_ref(j);
   i += make_rvalue_ref(i) - make_rvalue_ref(j);

   i *= j;
   i = i * j;
   i += make_rvalue_ref(i) * j;
   i += i * make_rvalue_ref(j);
   i += make_rvalue_ref(i) * make_rvalue_ref(j);

   i /= j;
   i = i / j;
   i += make_rvalue_ref(i) / j;
   i += i / make_rvalue_ref(j);
   i += make_rvalue_ref(i) / make_rvalue_ref(j);

   i %= j;
   i = i % j;
   i += make_rvalue_ref(i) % j;
   i += i % make_rvalue_ref(j);
   i += make_rvalue_ref(i) % make_rvalue_ref(j);

   i.swap(j);
   swap(i, j);
   using std::min;
   using std::max;
   i = max(i, j);
   j = min(i, j);
   
   j *= j * j * j;
#ifndef BOOST_MSVC
   i = gcd(i, j);
   i = lcm(i, j);
#endif
   i = pow(j, 2);
   i += powm(i, j, i + j);
   i += powm(i, j, 2);
   i += powm(i, 12, 2);
#ifndef BOOST_MSVC
   i = sqrt(i);
   i += sqrt(i, j);
#endif
   T q, r;
   divide_qr(i, j, q, r);
   i += integer_modulus(j, 3);
#ifndef BOOST_MSVC
   i += lsb(i);
   i += msb(i);
#endif
   i += abs(i);

   bool b = i == j;
   b &= i < j;
   b |= i > j;
   b |= i >= j;
   b &= i <= j;
   b &= i != j;
   b = i.compare(j);
   b |= i.is_zero();
   b |= i.sign() > 0;

   b |= bit_test(i, 5);
   bit_set(i, 6);
   bit_unset(i, 7);
   bit_flip(i, 8);

   i &= test_mixed(i, static_cast<signed char>(3));
   i &= test_mixed(i, static_cast<char>(3));
   i &= test_mixed(i, static_cast<unsigned char>(3));
   i &= test_mixed(i, static_cast<short>(3));
   i &= test_mixed(i, static_cast<unsigned short>(3));
   i &= test_mixed(i, static_cast<long>(3));
   i &= test_mixed(i, static_cast<unsigned long>(3));
   i &= test_mixed(i, static_cast<long long>(3));
   i &= test_mixed(i, static_cast<unsigned long long>(3));

   return i;
}

int main()
{
   using namespace boost::multiprecision;

   constexpr 
      uint1024_t i = get_constexpr<uint1024_t>();
   constexpr
      int1024_t j  = get_constexpr<int1024_t>();
   constexpr
      checked_uint1024_t ic = get_constexpr<checked_uint1024_t>();
   constexpr
      checked_int1024_t jc = get_constexpr<checked_int1024_t>();

   typedef number<cpp_int_backend<64, 64, unsigned_magnitude, unchecked, void> >   uint64_t;
   typedef number<cpp_int_backend<64, 64, signed_magnitude, unchecked, void> >     int64_t;
   typedef number<cpp_int_backend<64, 64, unsigned_magnitude, checked, void> >   checked_uint64_t;
   typedef number<cpp_int_backend<64, 64, signed_magnitude, checked, void> >     checked_int64_t;

   constexpr
      int64_t k = get_constexpr<int64_t>();
   constexpr
      uint64_t l = get_constexpr<uint64_t>();
   constexpr
      checked_int64_t kc = get_constexpr<checked_int64_t>();
   constexpr
      checked_uint64_t lc = get_constexpr<checked_uint64_t>();

   return 0;
}


#else

int main() { return 0; }

#endif

