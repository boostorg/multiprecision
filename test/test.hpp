///////////////////////////////////////////////////////////////
//  Copyright 2012 - 2025 John Maddock.
//  Copyright 2025 Christopher Kormanyos.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//

#ifndef BOOST_MULTIPRECISION_TEST_HPP
#define BOOST_MULTIPRECISION_TEST_HPP

#include <boost/detail/lightweight_test.hpp>
#include <boost/current_function.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/detail/standalone_config.hpp>

#include <limits>
#include <cmath>
#include <typeinfo>

namespace detail {

template <class T>
inline typename std::enable_if<!(boost::multiprecision::detail::is_unsigned<T>::value || boost::multiprecision::is_unsigned_number<T>::value), T>::type
abs(const T& a)
{
   return a < 0 ? -a : a;
}

template <class T>
inline typename std::enable_if<boost::multiprecision::detail::is_unsigned<T>::value || boost::multiprecision::is_unsigned_number<T>::value, T>::type
abs(const T& a)
{
   return a;
}

} // namespace detail

template <class T>
typename std::enable_if<boost::multiprecision::number_category<T>::value == boost::multiprecision::number_kind_integer, T>::type relative_error(T a, T b)
{
   return a > b ? a - b : b - a;
}

template <class T>
typename std::enable_if<!((boost::multiprecision::number_category<T>::value == boost::multiprecision::number_kind_integer) || boost::multiprecision::is_interval_number<T>::value), T>::type relative_error(T a, T b)
{
   using ::detail::abs;
   using std::abs;

   T min_val = (std::numeric_limits<T>::min)();
   T max_val = (std::numeric_limits<T>::max)();

   if ((a != 0) && (b != 0))
   {
      if (a == b)
         return 0;

      // TODO: use isfinite:
      if (abs(b) >= max_val)
      {
         if (abs(a) >= max_val)
            return 0; // one infinity is as good as another!
      }
      // If the result is denormalised, treat all denorms as equivalent:
      if ((a < min_val) && (a > 0))
         a = min_val;
      else if ((a > -min_val) && (a < 0))
         a = -min_val;
      if ((b < min_val) && (b > 0))
         b = min_val;
      else if ((b > -min_val) && (b < 0))
         b = -min_val;

      return (std::max)(abs(T((a - b) / a)), abs(T((a - b) / b))) / std::numeric_limits<T>::epsilon();
   }

   // Handle special case where one or both are zero:
   if (min_val == 0)
      return abs(T(a - b));
   if (abs(a) < min_val)
      a = min_val;
   if (abs(b) < min_val)
      b = min_val;

   return (std::max)(abs(T((a - b) / a)), abs(T((a - b) / b))) / std::numeric_limits<T>::epsilon();
}

template <class T>
typename std::enable_if<boost::multiprecision::is_interval_number<T>::value, T>::type relative_error(T a, T b)
{
   typename boost::multiprecision::component_type<T>::type am = median(a);
   typename boost::multiprecision::component_type<T>::type bm = median(b);
   return relative_error<typename boost::multiprecision::component_type<T>::type>(am, bm);
}

template <class T, class U>
typename std::conditional<std::is_convertible<T, U>::value, U, T>::type
relative_error(T a, U b)
{
   typedef typename std::conditional<std::is_convertible<T, U>::value, U, T>::type cast_type;
   return relative_error<cast_type>(static_cast<cast_type>(a), static_cast<cast_type>(b));
}

enum
{
   warn_on_fail,
   error_on_fail,
   abort_on_fail
};

template <class T>
inline T epsilon_of(const T&)
{
   static_assert(std::numeric_limits<T>::is_specialized, "No numeric_limits support");
   return std::numeric_limits<T>::is_integer ? static_cast<T>(1) : std::numeric_limits<T>::epsilon();
}

template <class T>
inline int digits_of(const T&)
{
   return std::numeric_limits<T>::is_specialized ? std::numeric_limits<T>::digits10 + 3 : std::numeric_limits<long double>::digits10 + 3;
}

inline std::ostream& report_where(const char* file, int line, const char* function)
{
   if (function)
      BOOST_LIGHTWEIGHT_TEST_OSTREAM << "In function: " << function << std::endl;
   BOOST_LIGHTWEIGHT_TEST_OSTREAM << file << ":" << line;
   return BOOST_LIGHTWEIGHT_TEST_OSTREAM;
}

#define BOOST_MP_REPORT_WHERE report_where(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION)

inline void report_severity(int severity)
{
   if (severity == error_on_fail)
      ++boost::detail::test_errors();
   else if (severity == abort_on_fail)
   {
      ++boost::detail::test_errors();
      abort();
   }
}

#define BOOST_MP_REPORT_SEVERITY(severity) report_severity(severity)

template <class E>
void report_unexpected_exception(const E& e, int severity, const char* file, int line, const char* function)
{
   report_where(file, line, function) << " Unexpected exception of type " << typeid(e).name() << std::endl;
   BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Errot message was: " << e.what() << std::endl;
   BOOST_MP_REPORT_SEVERITY(severity);
}

#ifdef BOOST_HAS_INT128

std::ostream& operator<<(std::ostream& os, boost::int128_type val)
{
   std::stringstream ss;
   ss << std::hex << "0x" << static_cast<std::uint64_t>(static_cast<boost::uint128_type>(val) >> 64) << static_cast<std::uint64_t>(val);
   return os << ss.str();
}

std::ostream& operator<<(std::ostream& os, boost::uint128_type val)
{
   std::stringstream ss;
   ss << std::hex << "0x" << static_cast<std::uint64_t>(val >> 64) << static_cast<std::uint64_t>(val);
   return os << ss.str();
}

#endif
#if defined(BOOST_HAS_FLOAT128) && !defined(BOOST_MP_HAVE_CSTDFLOAT)
std::ostream& operator<<(std::ostream& os, __float128 f)
{
   return os << static_cast<long double>(f);
}
#endif

#ifndef BOOST_NO_EXCEPTIONS
#define BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)                                       \
   catch (const std::exception& e)                                                          \
   {                                                                                        \
      report_unexpected_exception(e, severity, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION); \
   }                                                                                        \
   catch (...)                                                                              \
   {                                                                                        \
      std::cout << "Exception of unknown type was thrown" << std::endl;                     \
      report_severity(severity);                                                            \
   }
#define BOOST_MP_TEST_TRY try
#else
#define BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)
#define BOOST_MP_TEST_TRY
#endif

#define BOOST_CHECK_IMP(x, severity)                                                        \
   BOOST_MP_TEST_TRY                                                                        \
   {                                                                                        \
      if (x)                                                                                \
      {                                                                                     \
      }                                                                                     \
      else                                                                                  \
      {                                                                                     \
         BOOST_MP_REPORT_WHERE << " Failed predicate: " << BOOST_STRINGIZE(x) << std::endl; \
         BOOST_MP_REPORT_SEVERITY(severity);                                                \
      }                                                                                     \
   }                                                                                        \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_CHECK(x) BOOST_CHECK_IMP(x, error_on_fail)
#define BOOST_WARN(x) BOOST_CHECK_IMP(x, warn_on_fail)
#define BOOST_REQUIRE(x) BOOST_CHECK_IMP(x, abort_on_fail)

#define BOOST_CLOSE_IMP(x, y, tol, severity)                                                \
   BOOST_MP_TEST_TRY                                                                        \
   {                                                                                        \
      if (relative_error(x, y) > tol)                                                       \
      {                                                                                     \
         BOOST_MP_REPORT_WHERE << " Failed check for closeness: \n"                         \
                               << std::setprecision(digits_of(x)) << std::scientific        \
                               << "Value of LHS was: " << x << "\n"                         \
                               << "Value of RHS was: " << y << "\n"                         \
                               << std::setprecision(5) << std::fixed                        \
                               << "Relative error was: " << relative_error(x, y) << "eps\n" \
                               << "Tolerance was: " << tol << "eps" << std::endl;           \
         BOOST_MP_REPORT_SEVERITY(severity);                                                \
      }                                                                                     \
   }                                                                                        \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_EQUAL_IMP(x, y, severity)                                              \
   BOOST_MP_TEST_TRY                                                                 \
   {                                                                                 \
      if (!((x) == (y)))                                                             \
      {                                                                              \
         BOOST_MP_REPORT_WHERE << " Failed check for equality: \n"                   \
                               << std::setprecision(digits_of(x)) << std::scientific \
                               << "Value of LHS was: " << (x) << "\n"                \
                               << "Value of RHS was: " << (y) << "\n"                \
                               << std::setprecision(3) << std::endl;                 \
         BOOST_MP_REPORT_SEVERITY(severity);                                         \
      }                                                                              \
   }                                                                                 \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_NE_IMP(x, y, severity)                                                 \
   BOOST_MP_TEST_TRY                                                                 \
   {                                                                                 \
      if (!(x != y))                                                                 \
      {                                                                              \
         BOOST_MP_REPORT_WHERE << " Failed check for non-equality: \n"               \
                               << std::setprecision(digits_of(x)) << std::scientific \
                               << "Value of LHS was: " << x << "\n"                  \
                               << "Value of RHS was: " << y << "\n"                  \
                               << std::setprecision(3) << std::endl;                 \
         BOOST_MP_REPORT_SEVERITY(severity);                                         \
      }                                                                              \
   }                                                                                 \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_LT_IMP(x, y, severity)                                                 \
   BOOST_MP_TEST_TRY                                                                 \
   {                                                                                 \
      if (!(x < y))                                                                  \
      {                                                                              \
         BOOST_MP_REPORT_WHERE << " Failed check for less than: \n"                  \
                               << std::setprecision(digits_of(x)) << std::scientific \
                               << "Value of LHS was: " << x << "\n"                  \
                               << "Value of RHS was: " << y << "\n"                  \
                               << std::setprecision(3) << std::endl;                 \
         BOOST_MP_REPORT_SEVERITY(severity);                                         \
      }                                                                              \
   }                                                                                 \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_GT_IMP(x, y, severity)                                                 \
   BOOST_MP_TEST_TRY                                                                 \
   {                                                                                 \
      if (!(x > y))                                                                  \
      {                                                                              \
         BOOST_MP_REPORT_WHERE << " Failed check for greater than: \n"               \
                               << std::setprecision(digits_of(x)) << std::scientific \
                               << "Value of LHS was: " << x << "\n"                  \
                               << "Value of RHS was: " << y << "\n"                  \
                               << std::setprecision(3) << std::endl;                 \
         BOOST_MP_REPORT_SEVERITY(severity);                                         \
      }                                                                              \
   }                                                                                 \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_LE_IMP(x, y, severity)                                                 \
   BOOST_MP_TEST_TRY                                                                 \
   {                                                                                 \
      if (!(x <= y))                                                                 \
      {                                                                              \
         BOOST_MP_REPORT_WHERE << " Failed check for less-than-equal-to: \n"         \
                               << std::setprecision(digits_of(x)) << std::scientific \
                               << "Value of LHS was: " << x << "\n"                  \
                               << "Value of RHS was: " << y << "\n"                  \
                               << std::setprecision(3) << std::endl;                 \
         BOOST_MP_REPORT_SEVERITY(severity);                                         \
      }                                                                              \
   }                                                                                 \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_GE_IMP(x, y, severity)                                                 \
   BOOST_MP_TEST_TRY                                                                 \
   {                                                                                 \
      if (!(x >= y))                                                                 \
      {                                                                              \
         BOOST_MP_REPORT_WHERE << " Failed check for greater-than-equal-to \n"       \
                               << std::setprecision(digits_of(x)) << std::scientific \
                               << "Value of LHS was: " << x << "\n"                  \
                               << "Value of RHS was: " << y << "\n"                  \
                               << std::setprecision(3) << std::endl;                 \
         BOOST_MP_REPORT_SEVERITY(severity);                                         \
      }                                                                              \
   }                                                                                 \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#ifndef BOOST_NO_EXCEPTIONS
#define BOOST_MT_CHECK_THROW_IMP(x, E, severity)                                                                   \
   BOOST_MP_TEST_TRY                                                                                               \
   {                                                                                                               \
      x;                                                                                                           \
      BOOST_MP_REPORT_WHERE << " Expected exception not thrown in expression " << BOOST_STRINGIZE(x) << std::endl; \
      BOOST_MP_REPORT_SEVERITY(severity);                                                                          \
   }                                                                                                               \
   catch (const E&) {}                                                                                             \
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)
#else
#define BOOST_MT_CHECK_THROW_IMP(x, E, severity)       \
   static_cast<void>(x);                               \
   static_cast<void>(E);                               \
   static_cast<void>(severity)
#endif

#define BOOST_CHECK_CLOSE(x, y, tol) BOOST_CLOSE_IMP(x, y, ((tol / (100 * epsilon_of(x)))), error_on_fail)
#define BOOST_WARN_CLOSE(x, y, tol) BOOST_CLOSE_IMP(x, y, (tol / (100 * epsilon_of(x))), warn_on_fail)
#define BOOST_REQUIRE_CLOSE(x, y, tol) BOOST_CLOSE_IMP(x, y, (tol / (100 * epsilon_of(x))), abort_on_fail)

#define BOOST_CHECK_CLOSE_FRACTION(x, y, tol) BOOST_CLOSE_IMP(x, y, ((tol / (epsilon_of(x)))), error_on_fail)
#define BOOST_WARN_CLOSE_FRACTION(x, y, tol) BOOST_CLOSE_IMP(x, y, (tol / (epsilon_of(x))), warn_on_fail)
#define BOOST_REQUIRE_CLOSE_FRACTION(x, y, tol) BOOST_CLOSE_IMP(x, y, (tol / (epsilon_of(x))), abort_on_fail)

#define BOOST_CHECK_EQUAL(x, y) BOOST_EQUAL_IMP(x, y, error_on_fail)
#define BOOST_WARN_EQUAL(x, y) BOOST_EQUAL_IMP(x, y, warn_on_fail)
#define BOOST_REQUIRE_EQUAL(x, y) BOOST_EQUAL_IMP(x, y, abort_on_fail)

#define BOOST_CHECK_NE(x, y) BOOST_NE_IMP(x, y, error_on_fail)
#define BOOST_WARN_NE(x, y) BOOST_NE_IMP(x, y, warn_on_fail)
#define BOOST_REQUIRE_NE(x, y) BOOST_NE_IMP(x, y, abort_on_fail)

#define BOOST_CHECK_LT(x, y) BOOST_LT_IMP(x, y, error_on_fail)
#define BOOST_WARN_LT(x, y) BOOST_LT_IMP(x, y, warn_on_fail)
#define BOOST_REQUIRE_LT(x, y) BOOST_LT_IMP(x, y, abort_on_fail)

#define BOOST_CHECK_GT(x, y) BOOST_GT_IMP(x, y, error_on_fail)
#define BOOST_WARN_GT(x, y) BOOST_GT_IMP(x, y, warn_on_fail)
#define BOOST_REQUIRE_GT(x, y) BOOST_GT_IMP(x, y, abort_on_fail)

#define BOOST_CHECK_LE(x, y) BOOST_LE_IMP(x, y, error_on_fail)
#define BOOST_WARN_LE(x, y) BOOST_LE_IMP(x, y, warn_on_fail)
#define BOOST_REQUIRE_LE(x, y) BOOST_LE_IMP(x, y, abort_on_fail)

#define BOOST_CHECK_GE(x, y) BOOST_GE_IMP(x, y, error_on_fail)
#define BOOST_WARN_GE(x, y) BOOST_GE_IMP(x, y, warn_on_fail)
#define BOOST_REQUIRE_GE(x, y) BOOST_GE_IMP(x, y, abort_on_fail)

#define BOOST_CHECK_THROW(x, E) BOOST_MT_CHECK_THROW_IMP(x, E, error_on_fail)
#define BOOST_WARN_THROW(x, E) BOOST_MT_CHECK_THROW_IMP(x, E, warn_on_fail)
#define BOOST_REQUIRE_THROW(x, E) BOOST_MT_CHECK_THROW_IMP(x, E, abort_on_fail)

#endif // BOOST_MULTIPRECISION_TEST_HPP
