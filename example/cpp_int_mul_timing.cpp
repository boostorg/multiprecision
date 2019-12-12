///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2019.                        //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <iterator>
#include <vector>

#define USE_BOOST_MULTIPRECISION

#include <boost/multiprecision/cpp_int.hpp>

template <class clock_type>
struct stopwatch
{
public:
  typedef typename clock_type::duration duration_type;

  stopwatch() : m_start(clock_type::now()) { }

  stopwatch(const stopwatch& other) : m_start(other.m_start) { }

  stopwatch& operator=(const stopwatch& other)
  {
    m_start = other.m_start;
    return *this;
  }

  ~stopwatch() { }

  duration_type elapsed() const
  {
    return (clock_type::now() - m_start);
  }

  void reset()
  {
    m_start = clock_type::now();
  }

private:
  typename clock_type::time_point m_start;
};

class random_pcg32_fast_base
{
protected:
  template<typename ArithmeticType>
  static ArithmeticType rotr(const ArithmeticType& value_being_shifted,
                             const std::size_t     bits_to_shift)
  {
    const std::size_t left_shift_amount =
      std::numeric_limits<ArithmeticType>::digits - bits_to_shift;

    const ArithmeticType part1 = ArithmeticType(value_being_shifted >> bits_to_shift);
    const ArithmeticType part2 = ArithmeticType(value_being_shifted << left_shift_amount);

    return ArithmeticType(part1 | part2);
  }

  template<typename xtype,
           typename itype>
  struct xsh_rr_mixin
  {
    static xtype output(const itype internal_value)
    {
      using bitcount_t = std::size_t;

      constexpr bitcount_t bits         = bitcount_t(sizeof(itype) * 8U);
      constexpr bitcount_t xtypebits    = bitcount_t(sizeof(xtype) * 8U);
      constexpr bitcount_t sparebits    = bits - xtypebits;
      constexpr bitcount_t wantedopbits =   ((xtypebits >= 128U) ? 7U
                                          : ((xtypebits >=  64U) ? 6U
                                          : ((xtypebits >=  32U) ? 5U
                                          : ((xtypebits >=  16U) ? 4U
                                          :                        3U))));

      constexpr bitcount_t opbits       = ((sparebits >= wantedopbits) ? wantedopbits : sparebits);
      constexpr bitcount_t amplifier    = wantedopbits - opbits;
      constexpr bitcount_t mask         = (1ULL << opbits) - 1U;
      constexpr bitcount_t topspare     = opbits;
      constexpr bitcount_t bottomspare  = sparebits - topspare;
      constexpr bitcount_t xshift       = (topspare + xtypebits) / 2U;

      const bitcount_t rot =
        ((opbits != 0U) ? (bitcount_t(internal_value >> (bits - opbits)) & mask)
                        : 0U);

      const bitcount_t amprot = (rot << amplifier) & mask;

      const itype internal_value_xor = internal_value ^ itype(internal_value >> xshift);

      const xtype result = rotr(xtype(internal_value_xor >> bottomspare), amprot);

      return result;
    }
  };
};

class random_pcg32_fast : public random_pcg32_fast_base
{
private:
  using itype = std::uint64_t;

  static constexpr bool is_mcg = false;

  static constexpr itype default_multiplier = static_cast<itype>(6364136223846793005ULL);
  static constexpr itype default_increment  = static_cast<itype>(1442695040888963407ULL);

public:
  using result_type = std::uint32_t;

  static constexpr itype default_seed = static_cast<itype>(0xCAFEF00DD15EA5E5ULL);

  explicit random_pcg32_fast(const itype state = default_seed)
    : my_inc(default_increment),
      my_state(is_mcg ? state | itype(3U) : bump(state + increment())) { }

  random_pcg32_fast(const random_pcg32_fast& other)
    : my_inc  (other.my_inc),
      my_state(other.my_state) { }

  ~random_pcg32_fast() = default;

  random_pcg32_fast& operator=(const random_pcg32_fast& other)
  {
    if(this != &other)
    {
      my_inc   = other.my_inc;
      my_state = other.my_state;
    }

    return *this;
  }

  void seed(const itype state = default_seed)
  {
    my_inc = default_increment;

    my_state = (is_mcg ? state | itype(3U) : bump(state + increment()));
  }

  result_type operator()()
  {
    const result_type value =
      xsh_rr_mixin<result_type, itype>::output(base_generate0());

    return value;
  }

private:
  itype my_inc;
  itype my_state;

  itype multiplier() const
  {
    return default_multiplier;
  }

  itype increment() const
  {
    return default_increment;
  }

  itype bump(const itype state)
  {
    return itype(state * multiplier()) + increment();
  }

  itype base_generate0()
  {
    const itype old_state = my_state;

    my_state = bump(my_state);

    return old_state;
  }
};


template<typename UnsignedIntegralType,
         typename RandomEngineType>
void get_random_big_uint(RandomEngineType& rng, UnsignedIntegralType& u)
{
  using local_random_value_type = typename RandomEngineType::result_type;

  using local_uint_type = UnsignedIntegralType;

  constexpr std::size_t digits_of_uint___type = static_cast<std::size_t>(std::numeric_limits<local_uint_type>::digits);
  constexpr std::size_t digits_of_random_type = static_cast<std::size_t>(std::numeric_limits<local_random_value_type>::digits);

  u = local_uint_type(0U);

  for(std::size_t i = 0U; i < digits_of_uint___type; i += digits_of_random_type)
  {
    u <<= digits_of_random_type;

    const local_random_value_type r32 = rng();

    u |= r32;
  }
}

using big_uint_backend_type =
  boost::multiprecision::cpp_int_backend<4096,
                                         4096,
                                         boost::multiprecision::unsigned_magnitude,
                                         boost::multiprecision::unchecked,
                                         void>;

using big_uint_type = boost::multiprecision::number<big_uint_backend_type, boost::multiprecision::et_off>;

namespace local
{
  std::vector<big_uint_type> a(100000U);
  std::vector<big_uint_type> b(100000U);
}

int main()
{
  using clock_type = std::chrono::high_resolution_clock;

  const std::uintmax_t local_now =
    static_cast<std::uintmax_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(clock_type::now().time_since_epoch()).count());

  random_pcg32_fast rng(local_now);

  for(auto& u : local::a) { get_random_big_uint(rng, u); }
  for(auto& u : local::b) { get_random_big_uint(rng, u); }

  stopwatch<std::chrono::high_resolution_clock> my_stopwatch;

  my_stopwatch.reset();
  for(std::size_t i = 0U; i < local::a.size(); ++i)
  {
    local::a[i] *= local::b[i];
  }
  const float total_time = std::chrono::duration_cast<std::chrono::duration<float>>(my_stopwatch.elapsed()).count();

  std::cout << "(" << std::numeric_limits<big_uint_type>::digits
            << ") total_time: "
            << total_time << std::endl;

  // Time Boost.Multiprecision 10^5 mul schoolbook.
  // (512) total_time: 0.0274155
  // (1024) total_time: 0.110293
  // (2048) total_time: 0.390335
  // (4096) total_time: 1.45973
  // (8192) total_time: 5.42956
  // (16384) total_time: 21.2418
  // (32768) total_time: 83.5812

  // Time Boost.Multiprecision 10^5 mul Karatsuba.
  // (1024) total_time: 0.110478
  // (2048) total_time: 0.39095
  // (4096) total_time: 2.25638
  // (8192) total_time: 6.79038
  // (16384) total_time: 20.8101
  // (32768) total_time: XXX memory (maybe stack overflow from recursion)

  // Time ckormanyos proprietary 10^5 mul.
  // (1024) total_time: 0.0458317
  // (2048) total_time: 0.191055
  // (4096) total_time: 0.716233
  // (8192) total_time: 2.78224
  // (16384) total_time: 10.9777
  // (32768) total_time: 43.4864
}
