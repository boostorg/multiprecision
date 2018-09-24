///////////////////////////////////////////////////////////////
//  Copyright 2018 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_


#include <boost\multiprecision\cpp_int.hpp>
#include <iostream>

struct kiss_rand
{
   constexpr kiss_rand() : x(1234567890987654321ULL), y(362436362436362436ULL), z(1066149217761810ULL), c(123456123456123456ULL) {}
   constexpr kiss_rand(std::uint64_t seed) : x(seed), y(362436362436362436ULL), z(1066149217761810ULL), c(123456123456123456ULL) {}
   constexpr kiss_rand(std::uint64_t seed_x, std::uint64_t seed_y) : x(seed_x), y(seed_y), z(1066149217761810ULL), c(123456123456123456ULL) {}
   constexpr kiss_rand(std::uint64_t seed_x, std::uint64_t seed_y, std::uint64_t seed_z) : x(seed_x), y(seed_y), z(seed_z), c(123456123456123456ULL) {}

   constexpr std::uint64_t operator()()
   {
      return MWC() + XSH() + CNG();
   }
private:
   constexpr std::uint64_t MWC()
   {
      std::uint64_t t = (x << 58) + c;
      c = (x >> 6);
      x += t;
      c += (x < t);
      return x;
   }
   constexpr std::uint64_t XSH()
   {
      y ^= (y << 13);
      y ^= (y >> 17);
      return y ^= (y << 43);
   }
   constexpr std::uint64_t CNG()
   {
      return z = 6906969069LL * z + 1234567;
   }
   std::uint64_t x, y, z, c;
};

inline constexpr void hash_combine(std::uint64_t& h, std::uint64_t k)
{
   constexpr const std::uint64_t m = 0xc6a4a7935bd1e995uLL;
   constexpr const int r = 47;

   k *= m;
   k ^= k >> r;
   k *= m;

   h ^= k;
   h *= m;

   // Completely arbitrary number, to prevent 0's
   // from hashing to 0.
   h += 0xe6546b64;
}

template <std::size_t N>
inline constexpr std::uint64_t string_to_hash(const char(&s)[N])
{
   std::uint64_t hash(0);
   for (unsigned i = 0; i < N; ++i)
      hash_combine(hash, s[i]);
   return hash;
}

template <class UnsignedInteger>
struct multiprecision_generator
{
   typedef UnsignedInteger result_type;
   constexpr multiprecision_generator(std::uint64_t seed1) : m_gen64(seed1) {}
   constexpr multiprecision_generator(std::uint64_t seed1, std::uint64_t seed2) : m_gen64(seed1, seed2) {}
   constexpr multiprecision_generator(std::uint64_t seed1, std::uint64_t seed2, std::uint64_t seed3) : m_gen64(seed1, seed2, seed3) {}

   static constexpr result_type min()
   {
      return 0u;
   }
   static constexpr result_type max()
   {
      return ~result_type(0u);
   }
   constexpr result_type operator()()
   {
      result_type result(m_gen64());
      unsigned digits = 64;
      while (digits < std::numeric_limits<result_type>::digits)
      {
         result <<= 64;
         result |= m_gen64();
         digits += 64;
      }
      return result;
   }
private:
   kiss_rand m_gen64;
};

template <class UnsignedInteger>
constexpr UnsignedInteger random_value()
{
   std::uint64_t date_hash = string_to_hash(__DATE__);
   multiprecision_generator<UnsignedInteger> big_gen(date_hash);
   return big_gen();
}

int main()
{
   using namespace boost::multiprecision;

   std::cout << __DATE__ << std::endl;
   std::cout << __TIME__ << std::endl;

   std::cout << boost::hash_value(__DATE__) << std::endl;

   constexpr uint1024_t rand = random_value<uint1024_t>();

   return 0;
}
