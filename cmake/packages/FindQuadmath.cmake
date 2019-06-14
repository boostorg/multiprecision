# Module that checks whether the compiler supports the
# quadruple precision floating point math
#
# Sets the following variables:
# HAVE_QUAD
# QUADMATH_LIBRARIES
#
# perform tests
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CMakePushCheckState)
include(CheckCXXCompilerFlag)

if(NOT DEFINED USE_QUADMATH OR USE_QUADMATH)
  if(NOT DEFINED HAVE_EXTENDED_NUMERIC_LITERALS)
    check_cxx_compiler_flag("-Werror -fext-numeric-literals" HAVE_EXTENDED_NUMERIC_LITERALS)
  endif()

  if (HAVE_EXTENDED_NUMERIC_LITERALS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
  endif()

  cmake_push_check_state(RESET)
  list(APPEND CMAKE_REQUIRED_LIBRARIES "quadmath")
  CHECK_CXX_SOURCE_COMPILES("
#include <quadmath.h>

int main(void){
    __float128 foo = sqrtq(123.456);
    foo = FLT128_MIN;
}" QUADMATH_FOUND)
  cmake_pop_check_state()

  if (QUADMATH_FOUND)
    set(QUADMATH_LIBRARIES "quadmath")
    set(HAVE_QUAD "${QUADMATH_FOUND}")
  endif()
endif()

if (USE_QUADMATH AND NOT QUADMATH_FOUND)
  message(FATAL_ERROR "Quadruple precision math support was explicitly requested but is unavailable!")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Quadmath
  DEFAULT_MSG
  QUADMATH_LIBRARIES
  HAVE_QUAD
  )
