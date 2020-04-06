#include <array>
#include <atomic>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <thread>
#include <vector>

#include <boost/math/constants/constants.hpp>

#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_CPP_DEC_FLOAT       101
#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_GMP_FLOAT           102
#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_CPP_BIN_FLOAT       103
#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_MPFR_FLOAT          104

#if !defined(BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE)
#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_CPP_DEC_FLOAT
//#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_CPP_BIN_FLOAT
//#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_GMP_FLOAT
//#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_MPFR_FLOAT
#endif

// 501 digits, 10,000 values, 7 threads
// backend              calculation_time_concurrent      calculation_time_sequential
// cpp_dec_float                   12.3s                            47.8s
// cpp_bin_float                   14.8s                            58.5s
// gmp_float                        1.1s                             4.4s

#if  (BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE == BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_CPP_DEC_FLOAT)
#include <boost/multiprecision/cpp_dec_float.hpp>

using big_float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<501>,
                                                     boost::multiprecision::et_off>;

#elif (BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE == BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_CPP_BIN_FLOAT)
#include <boost/multiprecision/cpp_bin_float.hpp>

using big_float_type = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<501>,
                                                     boost::multiprecision::et_off>;

#elif  (BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE == BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_GMP_FLOAT)
#include <boost/multiprecision/gmp.hpp>

using big_float_type = boost::multiprecision::number<boost::multiprecision::gmp_float<501>,
                                                     boost::multiprecision::et_off>;

#elif  (BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE == BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_MPFR_FLOAT)
#include <boost/multiprecision/mpfr.hpp>

using big_float_type = boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<501>,
                                                     boost::multiprecision::et_off>;

#else
#error BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE is undefined.
#endif

namespace boost { namespace multiprecision { namespace exercise_threading {

namespace detail {

namespace my_concurrency {
template<typename index_type,
         typename callable_function_type>
void parallel_for(index_type             start,
                  index_type             end,
                  callable_function_type parallel_function)
{
  // Estimate the number of threads available.
  static const unsigned int number_of_threads_hint =
    std::thread::hardware_concurrency();

  static const unsigned int number_of_threads_total =
    ((number_of_threads_hint == 0U) ? 4U : number_of_threads_hint);

  // Use only 3/4 of the available cores.
  static const unsigned int number_of_threads = number_of_threads_total - (number_of_threads_total / 8U);

  std::cout << "Executing with " << number_of_threads << " threads" << std::endl;

  // Set the size of a slice for the range functions.
  index_type n = index_type(end - start) + index_type(1);

  index_type slice =
    static_cast<index_type>(std::round(n / static_cast<float>(number_of_threads)));

  slice = (std::max)(slice, index_type(1));

  // Inner loop.
  auto launch_range =
    [&parallel_function](index_type index_lo, index_type index_hi)
    {
      for(index_type i = index_lo; i < index_hi; ++i)
      {
        parallel_function(i);
      }
    };

  // Create the thread pool and launch the jobs.
  std::vector<std::thread> pool;

  pool.reserve(number_of_threads);

  index_type i1 = start;
  index_type i2 = (std::min)(index_type(start + slice), end);

  for(index_type i = 0U; ((index_type(i + index_type(1U)) < number_of_threads) && (i1 < end)); ++i)
  {
    pool.emplace_back(launch_range, i1, i2);

    i1 = i2;

    i2 = (std::min)(index_type(i2 + slice), end);
  }

  if(i1 < end)
  {
    pool.emplace_back(launch_range, i1, end);
  }

  // Wait for the jobs to finish.
  for(std::thread& thread_in_pool : pool)
  {
    if(thread_in_pool.joinable())
    {
      thread_in_pool.join();
    }
  }
}
} // namespace my_concurrency

template<typename FloatingPointType,
         typename UnsignedIntegralType>
FloatingPointType pown(const FloatingPointType& b, const UnsignedIntegralType& p)
{
  // Calculate (b ^ p).

  using local_floating_point_type    = FloatingPointType;
  using local_unsigned_integral_type = UnsignedIntegralType;

  local_floating_point_type result;

  if     (p == local_unsigned_integral_type(0U)) { result = local_floating_point_type(1U); }
  else if(p == local_unsigned_integral_type(1U)) { result = b; }
  else if(p == local_unsigned_integral_type(2U)) { result = b; result *= b; }
  else
  {
    result = local_floating_point_type(1U);

    local_floating_point_type y(b);

    for(local_unsigned_integral_type p_local(p); p_local != local_unsigned_integral_type(0U); p_local >>= 1U)
    {
      if((static_cast<unsigned>(p_local) & 1U) != 0U)
      {
        result *= y;
      }

      y *= y;
    }
  }

  return result;
}

float log_integral_asymptotic_approximation(const float x)
{
  // Compute an asymptotic approximation
  // of the log_integral function, li(x).

  using std::log;

  const float log_x = log(x);

  float sum  = 1.0F;
  float term = 1.0F;

  float min_term = (std::numeric_limits<float>::max)();

  // Perform the asymptotic expansion of li(x).
  for(std::uint_fast8_t k = 1U; k <= 64U; ++k)
  {
    term *= k;
    term /= log_x;

    if((k > 3U) && (term > min_term))
    {
      // This asymptotic expansion is actually divergent.
      // Good results can be obtained by stopping the
      // expansion when the series begins to diverge.

      break;
    }

    if(term < min_term)
    {
      min_term = term;
    }

    sum += term;
  }

  return (sum * x) / log_x;
}

template<typename unsigned_integral_type = std::uint32_t,
         const unsigned_integral_type maximum_value = unsigned_integral_type(UINTMAX_C(10006722))>
void compute_primes_via_sieve(std::vector<unsigned_integral_type>& primes)
{
  // Use a sieve algorithm to generate a table of primes.
  // In this sieve, the logic is inverted. A value
  // of true in the relevant index position means that
  // the number is *not* prime, whereas a value of false
  // indicates that the index position is prime.

  // For the prime number 10,006,721 (which is the 664999th prime),
  // see also D.N. Lehmer, "List of prime numbers from 1 to 10,006,721"
  // (Carnegie Institution of Washington, Washington D.C. 1914).

  // Some settings include the following.
  //          30 calculates             29 (which is the            10th prime, and also all primes up to this)
  //         542 calculates            541 (which is the           100th prime, and also all primes up to this)
  //        3572 calculates          3,571 (which is the           500th prime, and also all primes up to this)
  //        7920 calculates          7,919 (which is the         1,000th prime, and also all primes up to this)
  //       17390 calculates         17,389 (which is the         2,000th prime, and also all primes up to this)
  //       27450 calculates         27,449 (which is the         3,000th prime, and also all primes up to this)
  //      104730 calculates        104,729 (which is the        10,000th prime, and also all primes up to this)
  //     1299710 calculates      1,299,709 (which is the       100,000th prime, and also all primes up to this)
  //    10006722 calculates     10,006,721 (which is the       664,999th prime, and also all primes up to this)
  //    15485864 calculates     15,485,863 (which is the     1,000,000th prime, and also all primes up to this)
  //   179424674 calculates    179,424,673 (which is the    10,000,000th prime, and also all primes up to this)
  //  2038074744 calculates  2,038,074,743 (which is the   100,000,000th prime, and also all primes up to this)
  // 22801763490 calculates 22,801,763,489 (which is the 1,000,000,000th prime, and also all primes up to this)

  using std::floor;
  using std::sqrt;

  const unsigned_integral_type imax =
    static_cast<unsigned_integral_type>(sqrt(static_cast<float>(maximum_value)));

  // Create the sieve of primes.

  // Use a custom bitset to contain the sieve.
  // This saves a lot of storage space.
  std::vector<bool> sieve(maximum_value, false);

  // Use parallel processing in the loop to speed things up.

  for(unsigned_integral_type outer_index_i = 2U; outer_index_i < imax; ++outer_index_i)
  {
    if(sieve[outer_index_i] == false)
    {
      const unsigned_integral_type i2 = unsigned_integral_type(outer_index_i * outer_index_i);

      for(unsigned_integral_type inner_index_j = i2; inner_index_j < maximum_value; inner_index_j += outer_index_i)
      {
        sieve[inner_index_j] = true;
      }
    }
  }

  // Calculate the upper limit of the number of primes expected.
  // Use an asymptotic expansion of the log_integral function li(x).

  // See also the article "Prime-counting function" at:
  // http://en.wikipedia.org/wiki/Prime-counting_function

  const float log_intergal_of_max_val =
    log_integral_asymptotic_approximation(static_cast<float>(maximum_value));

  const unsigned_integral_type upper_limit_of_number_of_primes =
    static_cast<unsigned_integral_type>(floor(log_intergal_of_max_val));

  primes.resize(upper_limit_of_number_of_primes, 0U);

  // Fill the prime numbers into the data table
  // by extracting them from the sieve of primes.

  unsigned_integral_type prime_counter  = 2U;
  unsigned_integral_type running_number = 2U;

  for(unsigned_integral_type i = 2U; i < sieve.size(); ++i)
  {
    if(sieve[i] == false)
    {
      primes[prime_counter] = running_number;

      ++prime_counter;
    }

    ++running_number;
  }
}

const std::vector<std::uint32_t>& primes()
{
  static const std::vector<std::uint32_t> my_primes =
  []() -> std::vector<std::uint32_t>
  {
    std::vector<std::uint32_t> local_primes;

    // Get at least 10,000 primes.
    detail::compute_primes_via_sieve<std::uint32_t, std::uint32_t(104730)>(local_primes);

    // Get exactly 10,000 primes, remove the first 2 zero entries in the sieve.
    return std::vector<std::uint32_t>(local_primes.cbegin() +  2U,
                                      local_primes.cbegin() + (2U + 10000U));
  }();

  return my_primes;
}

} // namespace detail

template<typename FloatingPointType>
FloatingPointType log(const FloatingPointType& x)
{
  // Use an AGM method to compute the logarithm of x.

  // For values less than 1 invert the argument and
  // remember (in this case) to negate the result below.
  const bool b_negate = (x < 1);

  const FloatingPointType xx = ((b_negate == false) ? x : 1 / x);

  // Set a0 = 1
  // Set b0 = 4 / (x * 2^m) = 1 / (x * 2^(m - 2))

  FloatingPointType ak(1U);

  const float n_times_factor = static_cast<float>(static_cast<float>(std::numeric_limits<FloatingPointType>::digits10) * 1.67F);
  const float lgx_over_lg2   = std::log(static_cast<float>(xx)) / std::log(2.0F);

  std::int32_t m = static_cast<std::int32_t>(n_times_factor - lgx_over_lg2);

  // Ensure that the resulting power is non-negative.
  // Also enforce that m >= 8.
  m = (std::max)(m, static_cast<std::int32_t>(8));

  FloatingPointType bk = detail::pown(FloatingPointType(2), static_cast<std::uint32_t>(m));

  bk *= xx;
  bk  = 4 / bk;

  FloatingPointType ak_tmp(0U);

  using std::sqrt;

  // Determine the requested precision of the upcoming iteration in units of digits10.
  const FloatingPointType target_tolerance = sqrt(std::numeric_limits<FloatingPointType>::epsilon()) / 100;

  for(std::int32_t k = static_cast<std::int32_t>(0); k < static_cast<std::int32_t>(64); ++k)
  {
    using std::fabs;

    // Check for the number of significant digits to be
    // at least half of the requested digits. If at least
    // half of the requested digits have been achieved,
    // then break after the upcoming iteration.
    const bool break_after_this_iteration = (   (k > static_cast<std::int32_t>(4))
                                             && (fabs(1 - fabs(ak / bk)) < target_tolerance));

    ak_tmp  = ak;
    ak     += bk;
    ak     /= 2;

    if(break_after_this_iteration)
    {
      break;
    }

    bk *= ak_tmp;
    bk  = sqrt(bk);
  }

  // We are now finished with the AGM iteration for log(x).

  // Compute log(x) = {pi / [2 * AGM(1, 4 / 2^m)]} - (m * ln2)
  // Note at this time that (ak = bk) = AGM(...)

  // Retrieve the value of pi, divide by (2 * a) and subtract (m * ln2).
  const FloatingPointType result =
       boost::math::constants::pi<FloatingPointType>() / (ak * 2)
    - (boost::math::constants::ln_two<FloatingPointType>() * m);

  return ((b_negate == true) ? -result : result);
}

} } } // namespace boost::multiprecision::exercise_threading

template<typename FloatingPointType>
bool log_agm_concurrent(float& calculation_time)
{
  const std::size_t count = boost::multiprecision::exercise_threading::detail::primes().size();

  std::vector<FloatingPointType> log_results(count);
  std::vector<FloatingPointType> log_control(count);

  std::atomic_flag log_agm_lock = ATOMIC_FLAG_INIT;

  std::size_t concurrent_log_agm_count = 0U;

  const std::clock_t start = std::clock();

  boost::multiprecision::exercise_threading::detail::my_concurrency::parallel_for
  (
    std::size_t(0U),
    log_results.size(),
    [&log_results, &log_control, &concurrent_log_agm_count, &log_agm_lock](std::size_t i)
    {
      while(log_agm_lock.test_and_set()) { ; }
      ++concurrent_log_agm_count;
      if((concurrent_log_agm_count % 100U) == 0U)
      {
        std::cout << "log agm concurrent at index "
                  << concurrent_log_agm_count
                  << " of "
                  << log_results.size()
                  << ". Total processed so far: "
                  << std::fixed
                  << std::setprecision(1)
                  << (100.0F * float(concurrent_log_agm_count)) / float(log_results.size())
                  << "%."
                  << "\r";
      }
      log_agm_lock.clear();

      const FloatingPointType dx = (FloatingPointType(1U) / (boost::multiprecision::exercise_threading::detail::primes()[i]));
      const FloatingPointType  x = boost::math::constants::catalan<FloatingPointType>() + dx;

      const FloatingPointType lr = boost::multiprecision::exercise_threading::log(x);
      const FloatingPointType lc = boost::multiprecision::log(x);

      log_results[i] = lr;
      log_control[i] = lc;
    }
  );

  calculation_time = static_cast<float>(std::clock() - start) / static_cast<float>(CLOCKS_PER_SEC);

  std::cout << std::endl;

  std::cout << "Checking results concurrent: ";

  bool result_is_ok = true;

  for(std::size_t i = 0U; i < log_results.size(); ++i)
  {
    using std::fabs;

    const FloatingPointType close_fraction = fabs(1 - (log_results[i] / log_control[i]));

    result_is_ok &= (close_fraction < std::numeric_limits<FloatingPointType>::epsilon() * 1000000U);
  }

  std::cout << std::boolalpha << result_is_ok << std::endl;

  return result_is_ok;
}

template<typename FloatingPointType>
bool log_agm_sequential(float& calculation_time)
{
  const std::size_t count = boost::multiprecision::exercise_threading::detail::primes().size();

  std::vector<FloatingPointType> log_results(count);
  std::vector<FloatingPointType> log_control(count);

  std::atomic_flag log_agm_lock = ATOMIC_FLAG_INIT;

  const std::clock_t start = std::clock();

  for(std::size_t i = 0U; i < log_results.size(); ++i)
  {
    const std::size_t sequential_log_agm_count = i + 1U;

    if((sequential_log_agm_count % 100U) == 0U)
    {
      std::cout << "log agm sequential at index "
                << sequential_log_agm_count
                << " of "
                << log_results.size()
                << ". Total processed so far: "
                << std::fixed
                << std::setprecision(1)
                << (100.0F * float(sequential_log_agm_count)) / float(log_results.size())
                << "%."
                << "\r";
    }

    const FloatingPointType dx = (FloatingPointType(1U) / (boost::multiprecision::exercise_threading::detail::primes()[i]));
    const FloatingPointType  x = boost::math::constants::catalan<FloatingPointType>() + dx;

    log_results[i] = boost::multiprecision::exercise_threading::log(x);
    log_control[i] = boost::multiprecision::log(x);
  }

  calculation_time = static_cast<float>(std::clock() - start) / static_cast<float>(CLOCKS_PER_SEC);

  std::cout << std::endl;

  std::cout << "Checking results sequential: ";

  bool result_is_ok = true;

  for(std::size_t i = 0U; i < log_results.size(); ++i)
  {
    using std::fabs;

    const FloatingPointType close_fraction = fabs(1 - (log_results[i] / log_control[i]));

    result_is_ok &= (close_fraction < std::numeric_limits<FloatingPointType>::epsilon() * 1000000U);
  }

  std::cout << std::boolalpha << result_is_ok << std::endl;

  return result_is_ok;
}

int main()
{
  std::cout << "Calculating "
            << boost::multiprecision::exercise_threading::detail::primes().size()
            << " primes"
            << std::endl;

  float calculation_time_concurrent;
  const bool result_is_ok_concurrent = log_agm_concurrent<big_float_type>(calculation_time_concurrent);

  float calculation_time_sequential;
  const bool result_is_ok_sequential = log_agm_sequential<big_float_type>(calculation_time_sequential);

  std::cout << std::endl;

  std::cout << "result_is_ok_concurrent: "
            << std::boolalpha
            << result_is_ok_concurrent
            << ", calculation_time_concurrent: "
            << std::fixed
            << std::setprecision(1)
            << calculation_time_concurrent
            << "s"
            << std::endl;

  std::cout << "result_is_ok_sequential: "
            << std::boolalpha
            << result_is_ok_sequential
            << ", calculation_time_sequential: "
            << std::fixed
            << std::setprecision(1)
            << calculation_time_sequential
            << "s"
            << std::endl;

}
