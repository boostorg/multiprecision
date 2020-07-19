#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#
#
# Once done this will define
# QUADMATH_FOUND - system has Quadmath lib
# QUADMATH_INCLUDE_DIRS - the Quadmath include directory
# QUADMATH_LIBRARIES - Libraries needed to use Quadmath

include(FindPackageHandleStandardArgs)
include(CMakePushCheckState)
include(CheckCXXSourceCompiles)

if(QUADMATH_FOUND)
    find_path(QUADMATH_INCLUDE_DIRS NAMES quadmath.h PATHS /usr/local/lib /usr/local/include /usr/include
              /opt/loca/include)
    find_library(QUADMATH_LIBRARY NAMES libquadmath.a PATHS /usr/local/lib/gcc/8/ /opt/local/lib /usr/local/lib/)
else()
    find_path(QUADMATH_INCLUDE_DIRS NAMES quadmath.h PATHS /usr/local/lib /usr/local/include /usr/include
              /opt/loca/include)
    find_library(QUADMATH_LIBRARY NAMES libquadmath.a PATHS /usr/local/lib/gcc/8/ /opt/local/lib /usr/local/lib/)
endif()

if(NOT QUADMATH_INCLUDE_DIRS OR NOT QUADMATH_LIBRARY)
    cmake_push_check_state(RESET)
    list(APPEND CMAKE_REQUIRED_LIBRARIES "quadmath")
    check_cxx_source_compiles("
        #include <quadmath.h>
        int main(void){
            _float128 foo = ::sqrtq(123.456);
        }"
                              QUADMATH_USE_DIRECTLY)
    cmake_pop_check_state()
    if(QUADMATH_USE_DIRECTLY)
        set(QUADMATH_INCLUDE_DIRS "unused" CACHE PATH "" FORCE)
        set(QUADMATH_LIBRARY "quadmath" CACHE FILEPATH "" FORCE)
    endif()
endif()

find_package_handle_standard_args(Quadmath DEFAULT_MSG QUADMATH_LIBRARY QUADMATH_INCLUDE_DIRS)

mark_as_advanced(QUADMATH_INCLUDE_DIRS QUADMATH_LIBRARY)

if(QUADMATH_FOUND AND NOT TARGET Quadmath::quadmath)
    if(QUADMATH_USE_DIRECTLY)
        message(STATUS "libquadmath will be included and linked directly.")
        add_library(Quadmath::quadmath INTERFACE IMPORTED)
        set_target_properties(Quadmath::quadmath PROPERTIES INTERFACE_LINK_LIBRARIES "${QUADMATH_LIBRARY}")
    else()
        add_library(Quadmath::quadmath UNKNOWN IMPORTED)
        set_target_properties(Quadmath::quadmath PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${QUADMATH_INCLUDE_DIRS}"
                              IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                              IMPORTED_LOCATION "${QUADMATH_LIBRARY}")
    endif()
endif()
