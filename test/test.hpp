
#ifndef BOOST_MULTIPRECISION_TEST_HPP
#define BOOST_MULTIPRECISION_TEST_HPP

#include <limits>
#include <cmath>

#include <boost/detail/lightweight_test.hpp>
#include <boost/current_function.hpp>
#include <boost/static_assert.hpp>

template <class T>
T relative_error(T a, T b)
{
   T min_val = (std::numeric_limits<T>::min)();
   T max_val = (std::numeric_limits<T>::max)();

   if((a != 0) && (b != 0))
   {
      // TODO: use isfinite:
      if(fabs(b) >= max_val)
      {
         if(fabs(a) >= max_val)
            return 0;  // one infinity is as good as another!
      }
      // If the result is denormalised, treat all denorms as equivalent:
      if((a < min_val) && (a > 0))
         a = min_val;
      else if((a > -min_val) && (a < 0))
         a = -min_val;
      if((b < min_val) && (b > 0))
         b = min_val;
      else if((b > -min_val) && (b < 0))
         b = -min_val;
      return (std::max)(fabs((a-b)/a), fabs((a-b)/b)) / std::numeric_limits<T>::epsilon();
   }

   // Handle special case where one or both are zero:
   if(min_val == 0)
      return fabs(a-b);
   if(fabs(a) < min_val)
      a = min_val;
   if(fabs(b) < min_val)
      b = min_val;
   return (std::max)(fabs((a-b)/a), fabs((a-b)/b)) / std::numeric_limits<T>::epsilon();
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
   BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_specialized);
   return std::numeric_limits<T>::epsilon();
}

template <class T>
inline int digits_of(const T&)
{
   return std::numeric_limits<T>::is_specialized ? std::numeric_limits<T>::digits10 + 2 : std::numeric_limits<long double>::digits10 + 2;
}

inline std::ostream& report_where(const char* file, int line, const char* function)
{
   if(function)
      BOOST_LIGHTWEIGHT_TEST_OSTREAM << "In function: "<< function << std::endl;
   BOOST_LIGHTWEIGHT_TEST_OSTREAM << file << ":" << line;
   return BOOST_LIGHTWEIGHT_TEST_OSTREAM;
}

#define BOOST_MP_REPORT_WHERE report_where(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION)

inline void report_severity(int severity)
{
   if(severity == error_on_fail)
      ++boost::detail::test_errors();
   else if(severity == abort_on_fail)
   {
      ++boost::detail::test_errors();
      abort();
   }
}

#define BOOST_MP_REPORT_SEVERITY(severity) report_severity(severity)

template <class E>
void report_unexpected_exception(const E& e, int severity, const char* file, int line, const char* function)
{
   report_where(file, line, function)  << " Unexpected exception of type " << typeid(e).name() << std::endl;
   BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Errot message was: " << e.what() << std::endl;
   BOOST_MP_REPORT_SEVERITY(severity);
}

#define BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity) catch(const std::exception& e) {  report_unexpected_exception(e, severity, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION); }


#define BOOST_CHECK_IMP(x, severity)\
   try{ if(x){}else{\
   BOOST_MP_REPORT_WHERE << " Failed predicate: " << BOOST_STRINGIZE(x) << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   }BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_CHECK(x) BOOST_CHECK_IMP(x, error_on_fail)
#define BOOST_WARN(x)  BOOST_CHECK_IMP(x, warn_on_fail)
#define BOOST_REQUIRE(x)  BOOST_CHECK_IMP(x, abort_on_fail)

#define BOOST_CLOSE_IMP(x, y, tol, severity)\
   try{ if(relative_error(x, y) > tol){\
   BOOST_MP_REPORT_WHERE << " Failed check for closeness: \n" \
   << std::setprecision(digits_of(x)) << std::scientific\
   << "Value of LHS was: " << x << "\n"\
   << "Value of RHS was: " << y << "\n"\
   << std::setprecision(5) << std::fixed\
   << "Relative error was: " << relative_error(x, y) << "eps\n"\
   << "Tolerance was: " << tol << "eps" << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   }BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_EQUAL_IMP(x, y, severity)\
   try{ if(x != y){\
   BOOST_MP_REPORT_WHERE << " Failed check for equality: \n" \
   << std::setprecision(digits_of(x)) << std::scientific\
   << "Value of LHS was: " << x << "\n"\
   << "Value of RHS was: " << y << "\n"\
   << std::setprecision(3) << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   }BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_NE_IMP(x, y, severity)\
   try{ if(!(x != y)){\
   BOOST_MP_REPORT_WHERE << " Failed check for non-equality: \n" \
   << std::setprecision(digits_of(x)) << std::scientific\
   << "Value of LHS was: " << x << "\n"\
   << "Value of RHS was: " << y << "\n"\
   << std::setprecision(3) << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   }BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_LT_IMP(x, y, severity)\
   try{ if(!(x < y)){\
   BOOST_MP_REPORT_WHERE << " Failed check for less than: \n" \
   << std::setprecision(digits_of(x)) << std::scientific\
   << "Value of LHS was: " << x << "\n"\
   << "Value of RHS was: " << y << "\n"\
   << std::setprecision(3) << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   }BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_GT_IMP(x, y, severity)\
   try{ if(!(x > y)){\
   BOOST_MP_REPORT_WHERE << " Failed check for greater than: \n" \
   << std::setprecision(digits_of(x)) << std::scientific\
   << "Value of LHS was: " << x << "\n"\
   << "Value of RHS was: " << y << "\n"\
   << std::setprecision(3) << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   }BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_LE_IMP(x, y, severity)\
   try{ if(!(x <= y)){\
   BOOST_MP_REPORT_WHERE << " Failed check for less-than-equal-to: \n" \
   << std::setprecision(digits_of(x)) << std::scientific\
   << "Value of LHS was: " << x << "\n"\
   << "Value of RHS was: " << y << "\n"\
   << std::setprecision(3) << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   }BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_GE_IMP(x, y, severity)\
   try{ if(!(x >= y)){\
   BOOST_MP_REPORT_WHERE << " Failed check for greater-than-equal-to \n" \
   << std::setprecision(digits_of(x)) << std::scientific\
   << "Value of LHS was: " << x << "\n"\
   << "Value of RHS was: " << y << "\n"\
   << std::setprecision(3) << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   }BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_MT_CHECK_THROW_IMP(x, E, severity)\
   try{ \
      x;\
   BOOST_MP_REPORT_WHERE << " Expected exception not thrown in expression " << BOOST_STRINGIZE(x) << std::endl;\
   BOOST_MP_REPORT_SEVERITY(severity);\
   }\
   catch(const E&){}\
   BOOST_MP_UNEXPECTED_EXCEPTION_CHECK(severity)

#define BOOST_CHECK_CLOSE(x, y, tol) BOOST_CLOSE_IMP(x, y, ((tol / (100 * epsilon_of(x)))), error_on_fail)
#define BOOST_WARN_CLOSE(x, y, tol)  BOOST_CLOSE_IMP(x, y, (tol / (100 * epsilon_of(x))), warn_on_fail)
#define BOOST_REQUIRE_CLOSE(x, y, tol)  BOOST_CLOSE_IMP(x, y, (tol / (100 * epsilon_of(x))), abort_on_fail)

#define BOOST_CHECK_CLOSE_FRACTION(x, y, tol) BOOST_CLOSE_IMP(x, y, ((tol / (epsilon_of(x)))), error_on_fail)
#define BOOST_WARN_CLOSE_FRACTION(x, y, tol)  BOOST_CLOSE_IMP(x, y, (tol / (epsilon_of(x))), warn_on_fail)
#define BOOST_REQUIRE_CLOSE_FRACTION(x, y, tol)  BOOST_CLOSE_IMP(x, y, (tol / (epsilon_of(x))), abort_on_fail)

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

#endif
