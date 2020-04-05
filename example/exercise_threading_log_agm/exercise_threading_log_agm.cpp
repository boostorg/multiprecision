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

#if !defined(BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE)
#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_CPP_DEC_FLOAT
//#define BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_GMP_FLOAT
#endif

#if  (BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE == BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_CPP_DEC_FLOAT)
#include <boost/multiprecision/cpp_dec_float.hpp>

using big_float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<1001>,
                                                     boost::multiprecision::et_off>;

#elif  (BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_TYPE == BOOST_MULTIPRECISION_EXERCISE_THREADING_BACKEND_GMP_FLOAT)
#include <boost/multiprecision/gmp.hpp>

using big_float_type = boost::multiprecision::number<boost::multiprecision::gmp_float<2501>,
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

  // Set the size of a slice for the range functions.
  index_type n = index_type(end - start) + index_type(1);

  index_type slice =
    static_cast<index_type>(std::round(n / static_cast<double>(number_of_threads)));

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

  constexpr std::array<std::size_t, 1000U> primes =
  {{
       2,    3,    5,    7,   11,   13,   17,   19,   23,   29,   31,   37,   41,   43,   47,   53,   59,   61,   67,   71,
      73,   79,   83,   89,   97,  101,  103,  107,  109,  113,  127,  131,  137,  139,  149,  151,  157,  163,  167,  173,
     179,  181,  191,  193,  197,  199,  211,  223,  227,  229,  233,  239,  241,  251,  257,  263,  269,  271,  277,  281,
     283,  293,  307,  311,  313,  317,  331,  337,  347,  349,  353,  359,  367,  373,  379,  383,  389,  397,  401,  409,
     419,  421,  431,  433,  439,  443,  449,  457,  461,  463,  467,  479,  487,  491,  499,  503,  509,  521,  523,  541,
     547,  557,  563,  569,  571,  577,  587,  593,  599,  601,  607,  613,  617,  619,  631,  641,  643,  647,  653,  659,
     661,  673,  677,  683,  691,  701,  709,  719,  727,  733,  739,  743,  751,  757,  761,  769,  773,  787,  797,  809,
     811,  821,  823,  827,  829,  839,  853,  857,  859,  863,  877,  881,  883,  887,  907,  911,  919,  929,  937,  941,
     947,  953,  967,  971,  977,  983,  991,  997, 1009 ,1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069,
    1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223,
    1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373,
    1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511,
    1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657,
    1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811,
    1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987,
    1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129,
    2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287,
    2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423,
    2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617,
    2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741,
    2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903,
    2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079,
    3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181, 3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257,
    3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413,
    3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571,
    3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727,
    3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821, 3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907,
    3911, 3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989, 4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057,
    4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139, 4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231,
    4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409,
    4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583,
    4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657, 4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751,
    4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831, 4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933, 4937,
    4943, 4951, 4957, 4967, 4969, 4973, 4987, 4993, 4999, 5003, 5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087,
    5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179, 5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279,
    5281, 5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443,
    5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639,
    5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689, 5693, 5701, 5711, 5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791,
    5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857, 5861, 5867, 5869, 5879, 5881, 5897, 5903, 5923, 5927, 5939,
    5953, 5981, 5987, 6007, 6011, 6029, 6037, 6043, 6047, 6053, 6067, 6073, 6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133,
    6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217, 6221, 6229, 6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301,
    6311, 6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367, 6373, 6379, 6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473,
    6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563, 6569, 6571, 6577, 6581, 6599, 6607, 6619, 6637, 6653, 6659, 6661, 6673,
    6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733, 6737, 6761, 6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833,
    6841, 6857, 6863, 6869, 6871, 6883, 6899, 6907, 6911, 6917, 6947, 6949, 6959, 6961, 6967, 6971, 6977, 6983, 6991, 6997,
    7001, 7013, 7019, 7027, 7039, 7043, 7057, 7069, 7079, 7103, 7109, 7121, 7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207,
    7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253, 7283, 7297, 7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411,
    7417, 7433, 7451, 7457, 7459, 7477, 7481, 7487, 7489, 7499, 7507, 7517, 7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561,
    7573, 7577, 7583, 7589, 7591, 7603, 7607, 7621, 7639, 7643, 7649, 7669, 7673, 7681, 7687, 7691, 7699, 7703, 7717, 7723,
    7727, 7741, 7753, 7757, 7759, 7789, 7793, 7817, 7823, 7829, 7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919
  }};

} // namespace detail

template<typename FloatingPointType>
FloatingPointType log(const FloatingPointType& x)
{
  // For values less than 1 invert the argument and negate the result below.
  const bool b_negate = (x < 1);

  const FloatingPointType xx = ((b_negate == false) ? x : 1 / x);

  // Use an AGM method.

  // Set a0 = 1.
  // Set b0 = 4 / (*this * 2^m) = (1 / *this) / 2^(m - 2)

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

    ak_tmp   = ak;
    ak      += bk;
    ak      /= 2;

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

  // Retrieve the value of pi.
  FloatingPointType result = boost::math::constants::pi<FloatingPointType>();

  // Divide by 2 * a.
  ak     *= 2;
  result /= ak;

  // Subtract m * ln2.
  result -= (boost::math::constants::ln_two<FloatingPointType>() * m);

  if(b_negate)
  {
    result = -result;
  }

  return result;
}

} } } // namespace boost::multiprecision::exercise_threading

template<typename FloatingPointType>
bool log_agm_concurrent(float& calculation_time)
{
  constexpr std::size_t count = boost::multiprecision::exercise_threading::detail::primes.size();

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
      std::cout << "log agm concurrent at index "
                << concurrent_log_agm_count
                << " of "
                << log_results.size()
                << ". Total processed so far: "
                << std::fixed
                << std::setprecision(1)
                << (100.0 * double(concurrent_log_agm_count)) / double(log_results.size())
                << "%."
                << "\r";
      log_agm_lock.clear();

      const FloatingPointType dx = (FloatingPointType(1U) / (boost::multiprecision::exercise_threading::detail::primes[i]));
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
  constexpr std::size_t count = boost::multiprecision::exercise_threading::detail::primes.size();

  std::vector<FloatingPointType> log_results(count);
  std::vector<FloatingPointType> log_control(count);

  std::atomic_flag log_agm_lock = ATOMIC_FLAG_INIT;

  const std::clock_t start = std::clock();

  for(std::size_t i = 0U; i < log_results.size(); ++i)
  {
    const std::size_t sequential_log_agm_count = i + 1U;

    std::cout << "log agm sequential at index "
              << sequential_log_agm_count
              << " of "
              << log_results.size()
              << ". Total processed so far: "
              << std::fixed
              << std::setprecision(1)
              << (100.0 * double(sequential_log_agm_count)) / double(log_results.size())
              << "%."
              << "\r";

    const FloatingPointType dx = (FloatingPointType(1U) / (boost::multiprecision::exercise_threading::detail::primes[i]));
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
