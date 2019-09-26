#---------------------------------------------------------------------------#
# Copyright (c) 2018-2019 Nil Foundation AG
# Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2018-2019 Alexey Moskvin
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

include(CMTest)

find_package(GMP)
find_package(Quadmath)
find_package(TomMath)
find_package(MPFR)
find_package(MPFI)
find_package(MPC)
find_package(Eigen3)

if(BOOST_CMAKE)
    find_package(${CMAKE_WORKSPACE_NAME}_algorithm)
    find_package(${CMAKE_WORKSPACE_NAME}_chrono)
    find_package(${CMAKE_WORKSPACE_NAME}_exception)
    find_package(${CMAKE_WORKSPACE_NAME}_timer)
    find_package(${CMAKE_WORKSPACE_NAME}_numeric_ublas)
    find_package(${CMAKE_WORKSPACE_NAME}_filesystem)
    find_package(${CMAKE_WORKSPACE_NAME}_test)
    find_package(${CMAKE_WORKSPACE_NAME}_serialization)
    find_package(${CMAKE_WORKSPACE_NAME}_numeric_interval)
    find_package(${CMAKE_WORKSPACE_NAME}_detail)
    find_package(${CMAKE_WORKSPACE_NAME}_throw_exception)
    find_package(${CMAKE_WORKSPACE_NAME}_math)
    find_package(${CMAKE_WORKSPACE_NAME}_regexp)

    cm_test_link_libraries(${CMAKE_WORKSPACE_NAME}_multiprecision
                           ${CMAKE_WORKSPACE_NAME}::algorithm
                           ${CMAKE_WORKSPACE_NAME}::chrono
                           ${CMAKE_WORKSPACE_NAME}::exception
                           ${CMAKE_WORKSPACE_NAME}::timer
                           ${CMAKE_WORKSPACE_NAME}::numeric_ublas
                           ${CMAKE_WORKSPACE_NAME}::filesystem
                           ${CMAKE_WORKSPACE_NAME}::test
                           ${CMAKE_WORKSPACE_NAME}::serialization
                           ${CMAKE_WORKSPACE_NAME}::numeric_interval
                           ${CMAKE_WORKSPACE_NAME}::detail
                           ${CMAKE_WORKSPACE_NAME}::throw_exception
                           ${CMAKE_WORKSPACE_NAME}::math
                           ${CMAKE_WORKSPACE_NAME}::regexp
                           )
else()
    find_package(Boost REQUIRED COMPONENTS chrono exception timer filesystem
                 unit_test_framework serialization regex)

    cm_test_link_libraries(${CMAKE_WORKSPACE_NAME}_multiprecision
                           ${Boost_LIBRARIES})

    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include"
                        "${CMAKE_CURRENT_BINARY_DIR}/include"

                        ${Boost_INCLUDE_DIRS})
endif()

add_custom_target(multiprecision_test_suite_arithmetic_tests)
add_custom_target(multiprecision_test_suite_functions_and_limits)
add_custom_target(multiprecision_test_suite_conversions)
add_custom_target(multiprecision_test_suite_cpp_int_tests)
add_custom_target(multiprecision_test_suite_misc)
add_custom_target(multiprecision_test_suite_specfun)
add_custom_target(multiprecision_test_suite_concepts)
add_custom_target(multiprecision_test_suite_examples)
add_custom_target(multiprecision_test_suite_compile_fail)

# copyright John Maddock 2011
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt.

# HACK: Workaround broken includes
file(GLOB TEST_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/*.ipp)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
foreach(HEADER ${TEST_HEADERS})
    configure_file(${HEADER} ${CMAKE_CURRENT_BINARY_DIR}/libs/multiprecision/test/${HEADER}
                   @ONLY)
endforeach()

add_library(multiprecision_test_settings INTERFACE)
target_include_directories(multiprecision_test_settings INTERFACE ${CURRENT_TEST_SOURCES_DIR}/include)
cm_test_link_libraries(multiprecision_test_settings)

# We set these to make it easier to set up and test GMP and MPFR under Win32:
# Speed up compiles:
# We can't yet enable this - it breaks the STL in some tests...
#<toolset>msvc,<optimization>off:<cxxflags>-RTCc
#<toolset>msvc,<optimization>off:<define>_ALLOW_RTCc_IN_STL

# add_library(no_eh_support STATIC no_eh_support.cpp)
# cm_mark_as_test(no_eh_support)
# target_link_libraries(no_eh_support boost_throw_exception)
# cm_target_link_test_libs(no_eh_support)

if(GMP_FOUND)
    try_compile(GMP_COMPILED ${CMAKE_BINARY_DIR} ${CURRENT_TEST_SOURCES_DIR}/../config/has_gmp.cpp
                LINK_LIBRARIES ${GMP_LIBRARIES}
                CMAKE_FLAGS "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'" "-DINCLUDE_DIRECTORIES=${GMP_INCLUDE_DIRS}")
endif()

if(MPFR_FOUND)
    try_compile(MPFR_COMPILED ${CMAKE_BINARY_DIR} ${CURRENT_TEST_SOURCES_DIR}/../config/has_mpfr.cpp
                LINK_LIBRARIES ${MPFR_LIBRARIES}
                CMAKE_FLAGS "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'" "-DINCLUDE_DIRECTORIES=${MPFR_INCLUDE_DIRS}")
endif()

if(TOMMATH_FOUND)
    try_compile(TOMMATH_COMPILED ${CMAKE_BINARY_DIR} ${CURRENT_TEST_SOURCES_DIR}/../config/has_tommath.cpp
                LINK_LIBRARIES ${TomMath_LIBRARIES}
                CMAKE_FLAGS "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'" "-DINCLUDE_DIRECTORIES=${TomMath_INCLUDE_DIRS}")
endif()

try_compile(FLOAT128_COMPILED ${CMAKE_BINARY_DIR} ${CURRENT_TEST_SOURCES_DIR}/../config/has_float128.cpp)

if(MPFI_FOUND)
    try_compile(MPFI_COMPILED ${CMAKE_BINARY_DIR} ${CURRENT_TEST_SOURCES_DIR}/../config/has_mpfi.cpp
                LINK_LIBRARIES ${MPFI_LIBRARIES}
                CMAKE_FLAGS "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'" "-DINCLUDE_DIRECTORIES=${MPFI_INCLUDE_DIRS}")
endif()

if(Quadmath_FOUND)
    try_compile(INTEL_QUAD_COMPILED ${CMAKE_BINARY_DIR} ${CURRENT_TEST_SOURCES_DIR}/../config/has_intel_quad.cpp CMAKE_FLAGS
                "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'")
endif()

if(MPC_FOUND)
    try_compile(MPC_COMPILED ${CMAKE_BINARY_DIR} ${CURRENT_TEST_SOURCES_DIR}/../config/has_mpc.cpp
                LINK_LIBRARIES ${MPC_LIBRARIES}
                CMAKE_FLAGS "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'" "-DINCLUDE_DIRECTORIES=${MPC_INCLUDE_DIRS}")
endif()

if(EIGEN_FOUND)
    try_compile(EIGEN_COMPILED ${CMAKE_BINARY_DIR} ${CURRENT_TEST_SOURCES_DIR}/../config/has_eigen.cpp
                CMAKE_FLAGS "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'" "-DINCLUDE_DIRECTORIES=${EIGEN3_INCLUDE_DIRS}")
endif()

#cm_test(NAME GMP_MY SOURCES ${CURRENT_TEST_SOURCES_DIR}/../config/has_float128.cpp)
#target_link_libraries(GMP_MY ${Quadmath_LIBRARY})
#target_include_directories(GMP_MY PRIVATE ${Quadmath_INCLUDE_DIR})

add_library(no_eh_support SHARED ${CURRENT_TEST_SOURCES_DIR}/no_eh_test_support.cpp)

cm_test(NAME multiprecision_test_test_arithmetic_backend_concept SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_backend_concept.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_backend_concept no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_backend_concept)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_dec_float_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_dec_float_1.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_dec_float_1 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_dec_float_1)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_dec_float_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_dec_float_2.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_dec_float_2 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_dec_float_2)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_dec_float_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_dec_float_3.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_dec_float_3 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_dec_float_3)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_dec_float_3m SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_dec_float_3m.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_dec_float_3m no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_dec_float_3m)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_bin_float_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_bin_float_1.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_bin_float_1 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_bin_float_1)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_bin_float_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_bin_float_2.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_bin_float_2 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_bin_float_2)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_bin_float_2m SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_bin_float_2m.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_bin_float_2m no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_bin_float_2m)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_bin_float_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_bin_float_3.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_bin_float_3 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_bin_float_3)

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_arithmetic_mpf_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpf_50.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpf_50 ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpf_50 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpf_50)

    cm_test(NAME multiprecision_test_test_arithmetic_mpf SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpf.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpf ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpf PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpf)

    cm_test(NAME multiprecision_test_test_arithmetic_mpz SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpz.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpz ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpz PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpz)

    cm_test(NAME multiprecision_test_test_arithmetic_mpz_rat SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpz_rat.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpz_rat ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpz_rat PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpz_rat)

    cm_test(NAME multiprecision_test_test_arithmetic_mpz_br SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpz_br.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpz_br ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpz_br PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpz_br)

    cm_test(NAME multiprecision_test_test_arithmetic_mpq SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpq.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpq ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpq PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpq)
endif()

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_arithmetic_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpfr.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpfr ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpfr PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpfr)

    cm_test(NAME multiprecision_test_test_arithmetic_mpfr_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpfr_50.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpfr_50 ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpfr_50 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpfr_50)

    cm_test(NAME multiprecision_test_test_arithmetic_mpfr_50_static SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpfr_50_static.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpfr_50_static ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpfr_50_static PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpfr_50_static)
endif()

if(TOMMATH_COMPILED)
    cm_test(NAME multiprecision_test_test_arithmetic_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_tommath.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_tommath ${TomMath_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_tommath PRIVATE ${TomMath_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_tommath)

    cm_test(NAME multiprecision_test_test_arithmetic_tommath_rat SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_tommath_rat.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_tommath_rat ${TomMath_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_tommath_rat PRIVATE ${TomMath_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_tommath_rat)

    cm_test(NAME multiprecision_test_test_arithmetic_tommath_br SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_tommath_br.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_tommath_br ${TomMath_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_tommath_br PRIVATE ${TomMath_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_tommath_br)
endif()

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_1.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_1 no_eh_support)
if(MSVC)
    target_compile_options(multiprecision_test_test_arithmetic_cpp_int_1 PRIVATE /bigobj)
endif()
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_1)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_2.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_2 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_2)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_3.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_3 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_3)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_4.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_4 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_4)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_5 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_5.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_5 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_5)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_6 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_6.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_6 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_6)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_7 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_7.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_7 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_7)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_8 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_8.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_8 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_8)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_9 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_9.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_9 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_9)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_10 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_10.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_10 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_10)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_11 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_11.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_11 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_11)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_12 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_12.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_12 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_12)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_13 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_13.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_13 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_13)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_14 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_14.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_14 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_14)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_15 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_15.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_15 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_15)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_16 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_16.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_16 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_16)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_17 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_17.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_17 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_17)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_18 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_18.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_18 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_18)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_19 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_19.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_19 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_19)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_br SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_br.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_cpp_int_br no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_cpp_int_br)

cm_test(NAME multiprecision_test_test_arithmetic_ab_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_ab_1.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_ab_1 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_ab_1)

cm_test(NAME multiprecision_test_test_arithmetic_ab_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_ab_2.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_ab_2 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_ab_2)

cm_test(NAME multiprecision_test_test_arithmetic_ab_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_ab_3.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_ab_3 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_ab_3)

cm_test(NAME multiprecision_test_test_cpp_dec_float_round SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_dec_float_round.cpp)
target_link_libraries(multiprecision_test_test_cpp_dec_float_round no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_cpp_dec_float_round)

cm_test(NAME multiprecision_test_test_arithmetic_logged_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_logged_1.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_logged_1 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_logged_1)

cm_test(NAME multiprecision_test_test_arithmetic_logged_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_logged_2.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_logged_2 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_logged_2)

cm_test(NAME multiprecision_test_test_arithmetic_dbg_adptr1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_dbg_adptr1.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_dbg_adptr1 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_dbg_adptr1)

cm_test(NAME multiprecision_test_test_arithmetic_dbg_adptr1m SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_dbg_adptr1m.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_dbg_adptr1m no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_dbg_adptr1m)

cm_test(NAME multiprecision_test_test_arithmetic_dbg_adptr2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_dbg_adptr2.cpp)
target_link_libraries(multiprecision_test_test_arithmetic_dbg_adptr2 no_eh_support)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_dbg_adptr2)

if(MPFI_COMPILED)
    cm_test(NAME multiprecision_test_test_arithmetic_mpfi_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpfi_50.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpfi_50 ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_arithmetic_mpfi_50 PRIVATE ${MPFI_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpfi_50)
endif()

if(INTEL_QUAD_COMPILED)
    cm_test(NAME multiprecision_test_test_arithmetic_float_128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_float_128.cpp)
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_float_128)
endif()

if(FLOAT128_COMPILED)
    cm_test(NAME multiprecision_test_test_arithmetic_float_128_quad SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_float_128.cpp)
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_float_128_quad)
endif()

if(MPC_COMPILED)
    cm_test(NAME multiprecision_test_test_arithmetic_mpc SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpc.cpp)
    target_link_libraries(multiprecision_test_test_arithmetic_mpc ${MPC_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_arithmetic_mpc PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_mpc)

    cm_test(NAME multiprecision_test_test_mpfr_mpc_precisions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mpfr_mpc_precisions.cpp)
    target_link_libraries(multiprecision_test_test_mpfr_mpc_precisions ${MPC_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_mpfr_mpc_precisions PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_mpfr_mpc_precisions)
endif()
if(MPFI_COMPILED)
    cm_test(NAME multiprecision_test_test_mpfi_precisions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mpfi_precisions.cpp)
    target_link_libraries(multiprecision_test_test_mpfi_precisions ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_mpfi_precisions PRIVATE ${MPFI_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_mpfi_precisions)
endif()

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_mpf_precisions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mpf_precisions.cpp)
    target_link_libraries(multiprecision_test_test_mpf_precisions ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_mpf_precisions PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_mpf_precisions)
endif()

if((MPC_COMPILED) AND (FLOAT128_COMPILED))
    cm_test(NAME multiprecision_test_test_complex SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_complex.cpp)
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_complex)
endif()

cm_test(NAME multiprecision_test_test_arithmetic_complex_adaptor SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_complex_adaptor.cpp)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_complex_adaptor)
cm_test(NAME multiprecision_test_test_arithmetic_complex_adaptor_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_complex_adaptor_2.cpp)
add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_arithmetic_complex_adaptor_2)

if(FLOAT128_COMPILED)
    cm_test(NAME multiprecision_test_test_arithmetic_complex128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_complex128.cpp)
    add_dependencies(multiprecision_test_suite_arithmetic_tests multiprecision_test_test_complex128)
endif()

set(LOCAL_SOURCES test_exp.cpp test_log.cpp test_pow.cpp test_sinh.cpp test_sqrt.cpp test_cosh.cpp test_tanh.cpp test_sin.cpp test_cos.cpp test_tan.cpp test_asin.cpp test_acos.cpp test_atan.cpp test_round.cpp test_fpclassify.cpp test_sf_import_c99.cpp)
foreach(FILE ${LOCAL_SOURCES})
    get_filename_component(NAME ${FILE} NAME_WE)
    if(GMP_COMPILED)
        cm_test(NAME multiprecision_test_${NAME}_mpf50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
        target_compile_definitions(multiprecision_test_${NAME}_mpf50 PUBLIC -DTEST_MPF_50)
        target_link_libraries(multiprecision_test_${NAME}_mpf50 ${GMP_LIBRARIES} no_eh_support)
        target_include_directories(multiprecision_test_${NAME}_mpf50 PRIVATE ${GMP_INCLUDE_DIRS})
        add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_${NAME}_mpf50)
    endif()
    if(MPFR_COMPILED)
        cm_test(NAME multiprecision_test_${NAME}_mpfr50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
        target_compile_definitions(multiprecision_test_${NAME}_mpfr50 PUBLIC -DTEST_MPFR_50)
        target_link_libraries(multiprecision_test_${NAME}_mpfr50 ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
        target_include_directories(multiprecision_test_${NAME}_mpfr50 PRIVATE ${GMP_INCLUDE_DIRS})
        add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_${NAME}_mpfr50)
    endif()
    if(MPFI_COMPILED)
        cm_test(NAME multiprecision_test_${NAME}_mpfi50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
        target_compile_definitions(multiprecision_test_${NAME}_mpfi50 PUBLIC -DTEST_MPFI_50)
        target_link_libraries(multiprecision_test_${NAME}_mpfi50 ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
        target_include_directories(multiprecision_test_${NAME}_mpfi50 PRIVATE ${MPFI_INCLUDE_DIRS})
        add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_${NAME}_mpfi50)
    endif()

    cm_test(NAME multiprecision_test_${NAME}_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
    target_compile_definitions(multiprecision_test_${NAME}_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)
    target_link_libraries(multiprecision_test_${NAME}_cpp_dec_float no_eh_support)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_${NAME}_cpp_dec_float)

    cm_test(NAME multiprecision_test_${NAME}_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
    target_compile_definitions(multiprecision_test_${NAME}_cpp_bin_float PUBLIC -DTEST_CPP_BIN_FLOAT)
    target_link_libraries(multiprecision_test_${NAME}_cpp_bin_float no_eh_support)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_${NAME}_cpp_bin_float)

    if(FLOAT128_COMPILED)
        cm_test(NAME multiprecision_test_${NAME}_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
        target_compile_definitions(multiprecision_test_${NAME}_float128 PUBLIC -DTEST_FLOAT128)
        target_link_libraries(multiprecision_test_${NAME}_float128 no_eh_support)
        add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_${NAME}_float128)
    endif()

    if(INTEL_QUAD_COMPILED)
        cm_test(NAME multiprecision_test_${NAME}_intel_quad SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
        target_compile_definitions(multiprecision_test_${NAME}_intel_quad PUBLIC -DTEST_FLOAT128)
        target_link_libraries(multiprecision_test_${NAME}_intel_quad no_eh_support)
        add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_${NAME}_intel_quad)
    endif()
endforeach()

cm_test(NAME multiprecision_test_test_numeric_limits_backend_concept SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
target_compile_definitions(multiprecision_test_test_numeric_limits_backend_concept PUBLIC -DTEST_BACKEND)
target_link_libraries(multiprecision_test_test_numeric_limits_backend_concept no_eh_support)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_backend_concept)

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_numeric_limits_mpf50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpf50 PUBLIC -DTEST_MPF_50)
    target_link_libraries(multiprecision_test_test_numeric_limits_mpf50 ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_numeric_limits_mpf50 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_mpf50)

    cm_test(NAME multiprecision_test_test_numeric_limits_mpf SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpf PUBLIC -DTEST_MPF)
    target_link_libraries(multiprecision_test_test_numeric_limits_mpf ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_numeric_limits_mpf PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_mpf)

    cm_test(NAME multiprecision_test_test_numeric_limits_mpz SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpz PUBLIC -DTEST_MPZ)
    target_link_libraries(multiprecision_test_test_numeric_limits_mpz ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_numeric_limits_mpz PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_mpz)

    cm_test(NAME multiprecision_test_test_numeric_limits_mpq SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpq PUBLIC -DTEST_MPQ)
    target_link_libraries(multiprecision_test_test_numeric_limits_mpq ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_numeric_limits_mpq PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_mpq)
endif()

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_numeric_limits_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpfr PUBLIC -DTEST_MPFR)
    target_link_libraries(multiprecision_test_test_numeric_limits_mpfr ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_numeric_limits_mpfr PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_mpfr)

    cm_test(NAME multiprecision_test_test_numeric_limits_mpfr_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpfr_50 PUBLIC -DTEST_MPFR_50)
    target_link_libraries(multiprecision_test_test_numeric_limits_mpfr_50 ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_numeric_limits_mpfr_50 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_mpfr_50)
endif()

cm_test(NAME multiprecision_test_test_numeric_limits_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
target_compile_definitions(multiprecision_test_test_numeric_limits_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)
target_link_libraries(multiprecision_test_test_numeric_limits_cpp_dec_float no_eh_support)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_cpp_dec_float)

cm_test(NAME multiprecision_test_test_numeric_limits_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
target_compile_definitions(multiprecision_test_test_numeric_limits_cpp_bin_float PUBLIC -DTEST_CPP_BIN_FLOAT)
target_link_libraries(multiprecision_test_test_numeric_limits_cpp_bin_float no_eh_support)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_cpp_bin_float)

if(TOMMATH_COMPILED)
    cm_test(NAME multiprecision_test_test_numeric_limits_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_tommath PUBLIC -DTEST_TOMMATH)
    target_link_libraries(multiprecision_test_test_numeric_limits_tommath ${TomMath_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_numeric_limits_tommath PRIVATE ${TomMath_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_tommath)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    cm_test(NAME multiprecision_test_test_numeric_limits_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_cpp_int PUBLIC -DTEST_CPP_INT)
    target_link_libraries(multiprecision_test_test_numeric_limits_cpp_int no_eh_support)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_cpp_int)
endif()

if(MPFI_COMPILED)
    cm_test(NAME multiprecision_test_test_numeric_limits_mpfi_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpfi_50 PUBLIC -DTEST_MPFI_50)
    target_link_libraries(multiprecision_test_test_numeric_limits_mpfi_50 ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_numeric_limits_mpfi_50 PRIVATE ${MPFI_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_mpfi_50)
endif()

if(FLOAT128_COMPILED)
    cm_test(NAME multiprecision_test_test_numeric_limits_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_float128 PUBLIC -DTEST_FLOAT128)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_float128)
endif()

if((INTEL_QUAD_COMPILED) AND (FLOAT128_COMPILED))
    cm_test(NAME multiprecision_test_test_numeric_limits_intel_quad SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_intel_quad PUBLIC -DTEST_FLOAT128)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_numeric_limits_intel_quad)
endif()

cm_test(NAME multiprecision_test_test_sf_import_c99_cpp_dec_float_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_sf_import_c99.cpp)
target_compile_definitions(multiprecision_test_test_sf_import_c99_cpp_dec_float_2 PUBLIC -DTEST_CPP_DEC_FLOAT_2)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_sf_import_c99_cpp_dec_float_2)

cm_test(NAME multiprecision_test_test_sf_import_c99_cpp_dec_float_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_sf_import_c99.cpp)
target_compile_definitions(multiprecision_test_test_sf_import_c99_cpp_dec_float_3 PUBLIC -DTEST_CPP_DEC_FLOAT_3)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_sf_import_c99_cpp_dec_float_3)

cm_test(NAME multiprecision_test_test_sf_import_c99_cpp_dec_float_4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_sf_import_c99.cpp)
target_compile_definitions(multiprecision_test_test_sf_import_c99_cpp_dec_float_4 PUBLIC -DTEST_CPP_DEC_FLOAT_4)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_sf_import_c99_cpp_dec_float_4)

cm_test(NAME multiprecision_test_test_sf_import_c99_cpp_dec_float_5 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_sf_import_c99.cpp)
target_compile_definitions(multiprecision_test_test_sf_import_c99_cpp_dec_float_5 PUBLIC -DTEST_CPP_DEC_FLOAT_5)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_sf_import_c99_cpp_dec_float_5)

cm_test(NAME multiprecision_test_test_sf_import_c99_cpp_dec_float_6 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_sf_import_c99.cpp)
target_compile_definitions(multiprecision_test_test_sf_import_c99_cpp_dec_float_6 PUBLIC -DTEST_CPP_DEC_FLOAT_6)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_sf_import_c99_cpp_dec_float_6)

cm_test(NAME multiprecision_test_test_sf_import_c99_cpp_bin_float_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_sf_import_c99.cpp)
target_compile_definitions(multiprecision_test_test_sf_import_c99_cpp_bin_float_2 PUBLIC -DTEST_CPP_BIN_FLOAT_2)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_sf_import_c99_cpp_bin_float_2)

cm_test(NAME multiprecision_test_test_sf_import_c99_cpp_bin_float_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_sf_import_c99.cpp)
target_compile_definitions(multiprecision_test_test_sf_import_c99_cpp_bin_float_3 PUBLIC -DTEST_CPP_BIN_FLOAT_2)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_sf_import_c99_cpp_bin_float_3)

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_move_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
    target_compile_definitions(multiprecision_test_test_move_mpfr PUBLIC -DTEST_MPFR)
    target_link_libraries(multiprecision_test_test_move_mpfr ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_move_mpfr)
endif()

if(MPC_COMPILED)
    cm_test(NAME multiprecision_test_test_move_mpc SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
    target_compile_definitions(multiprecision_test_test_move_mpc PUBLIC -DTEST_MPC)
    target_link_libraries(multiprecision_test_test_move_mpc ${MPC_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_move_mpc)
endif()

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_move_gmp SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
    target_compile_definitions(multiprecision_test_test_move_gmp PUBLIC -DTEST_GMP)
    target_link_libraries(multiprecision_test_test_move_gmp ${GMP_LIBRARIES} no_eh_support)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_move_gmp)
endif()

if(TOMMATH_COMPILED)
    cm_test(NAME multiprecision_test_test_move_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
    target_compile_definitions(multiprecision_test_test_move_tommath PUBLIC -DTEST_TOMMATH)
    target_link_libraries(multiprecision_test_test_move_tommath ${TomMath_LIBRARIES} no_eh_support)
    add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_move_tommath)
endif()

cm_test(NAME multiprecision_test_test_move_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
target_compile_definitions(multiprecision_test_test_move_cpp_int PUBLIC -DTEST_CPP_INT)
target_link_libraries(multiprecision_test_test_move_cpp_int no_eh_support)
add_dependencies(multiprecision_test_suite_functions_and_limits multiprecision_test_test_move_cpp_int)

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_gmp_conversions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_gmp_conversions.cpp)
    target_link_libraries(multiprecision_test_test_gmp_conversions ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_gmp_conversions PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_gmp_conversions)
endif()

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_mpfr_conversions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mpfr_conversions.cpp)
    target_link_libraries(multiprecision_test_test_mpfr_conversions ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_mpfr_conversions PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_mpfr_conversions)
endif()

if(MPC_COMPILED)
    cm_test(NAME multiprecision_test_test_mpc_conversions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mpfr_conversions.cpp)
    target_link_libraries(multiprecision_test_test_mpc_conversions ${MPC_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_mpc_conversions PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_mpc_conversions)
endif()

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_constants_mpf50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_constants.cpp)
    target_compile_definitions(multiprecision_test_test_constants_mpf50 PUBLIC -DTEST_MPF_50)
    target_link_libraries(multiprecision_test_test_constants_mpf50 ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_constants_mpf50 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_constants_mpf50)
endif()

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_constants_mpfr_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_constants.cpp)
    target_compile_definitions(multiprecision_test_test_constants_mpfr_50 PUBLIC -DTEST_MPFR_50)
    target_link_libraries(multiprecision_test_test_constants_mpfr_50 ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_constants_mpfr_50 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_constants_mpfr_50)
endif()

cm_test(NAME multiprecision_test_test_constants_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_constants.cpp)
target_compile_definitions(multiprecision_test_test_constants_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)
target_link_libraries(multiprecision_test_test_constants_cpp_dec_float no_eh_support)
add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_constants_cpp_dec_float)

cm_test(NAME multiprecision_test_test_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_test.cpp)

cm_test(NAME multiprecision_test_test_cpp_int_lit SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_lit.cpp)
target_link_libraries(multiprecision_test_test_cpp_int_lit no_eh_support)
add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_cpp_int_lit)

if((GMP_COMPILED) AND (MPFR_COMPILED) AND (MPFI_COMPILED) AND (TOMMATH_COMPILED) AND (FLOAT128_COMPILED))
    cm_test(NAME multiprecision_test_test_convert_from_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_cpp_int.cpp)
    target_link_libraries(multiprecision_test_test_convert_from_cpp_int ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_cpp_int PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_cpp_int)

    cm_test(NAME multiprecision_test_test_convert_from_mpz_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_mpz_int.cpp)
    target_link_libraries(multiprecision_test_test_convert_from_mpz_int ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_mpz_int PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_mpz_int)

    cm_test(NAME multiprecision_test_test_convert_from_tom_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_tom_int.cpp)
    target_link_libraries(multiprecision_test_test_convert_from_tom_int ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_tom_int PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_tom_int)

    cm_test(NAME multiprecision_test_test_convert_from_cpp_rational SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_cpp_rational.cpp)
    target_link_libraries(multiprecision_test_test_convert_from_cpp_rational ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_cpp_rational PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_cpp_rational)

    cm_test(NAME multiprecision_test_test_convert_from_gmp_rational SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_gmp_rational.cpp)
    target_link_libraries(test_convert_from_gmp_rational ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_gmp_rational PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_gmp_rational)

    cm_test(NAME multiprecision_test_test_convert_from_tom_rational SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_tom_rational.cpp)
    target_link_libraries(test_convert_from_tom_rational ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_tom_rational PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_tom_rational)

    cm_test(NAME multiprecision_test_test_convert_from_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_cpp_bin_float.cpp)
    target_link_libraries(test_convert_from_cpp_bin_float ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_cpp_bin_float PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_cpp_bin_float)

    cm_test(NAME multiprecision_test_test_convert_from_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_cpp_dec_float.cpp)
    target_link_libraries(test_convert_from_cpp_dec_float ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_cpp_dec_float PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_cpp_dec_float)

    cm_test(NAME multiprecision_test_test_convert_from_mpf_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_mpf_float.cpp)
    target_link_libraries(test_convert_from_mpf_float ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_mpf_float PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_mpf_float)

    cm_test(NAME multiprecision_test_test_convert_from_mpfr_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_mpfr_float.cpp)
    target_link_libraries(test_convert_from_mpfr_float ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_mpfr_float PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_mpfr_float)

    cm_test(NAME multiprecision_test_test_convert_from_mpfi_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_mpfi_float.cpp)
    target_link_libraries(test_convert_from_mpfi_float ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_mpfi_float PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_mpfi_float)

    cm_test(NAME multiprecision_test_test_convert_from_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_convert_from_float128.cpp)
    target_link_libraries(test_convert_from_float128 ${QUADMATH_LIBRARIES} ${TomMath_LIBRARIES}
                          ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_convert_from_float128 PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_convert_from_float128)

endif()

cm_test(NAME multiprecision_test_test_cpp_bin_float_conv SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_bin_float_conv.cpp)
add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_cpp_bin_float_conv)

cm_test(NAME multiprecision_test_test_cpp_bin_float_io_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_bin_float_io.cpp)
target_compile_definitions(multiprecision_test_test_cpp_bin_float_io_1 PUBLIC -DTEST1 -DTEST_CPP_BIN_FLOAT)
add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_cpp_bin_float_io_1)

cm_test(NAME multiprecision_test_test_cpp_bin_float_io_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_bin_float_io.cpp)
target_compile_definitions(multiprecision_test_test_cpp_bin_float_io_2 PUBLIC -DTEST2 -DTEST_CPP_BIN_FLOAT)
add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_cpp_bin_float_io_2)

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_bin_float.cpp)
    target_compile_definitions(multiprecision_test_test_cpp_bin_float PUBLIC -DTEST_MPFR)
    target_link_libraries(multiprecision_test_test_cpp_bin_float ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_cpp_bin_float PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_cpp_bin_float)
endif()

cm_test(NAME multiprecision_test_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp)
target_compile_definitions(multiprecision_test_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)
target_link_libraries(multiprecision_test_cpp_dec_float no_eh_support)
add_dependencies(multiprecision_test_suite_conversions multiprecision_test_cpp_dec_float)

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_float_io_mpf SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp)
    target_compile_definitions(multiprecision_test_test_float_io_mpf PUBLIC -DTEST_MPF_50)
    target_link_libraries(multiprecision_test_test_float_io_mpf ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_float_io_mpf PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_float_io_mpf)
endif()

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_float_io_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp)
    target_compile_definitions(multiprecision_test_test_float_io_mpfr PUBLIC -DTEST_MPFR_50)
    target_link_libraries(multiprecision_test_test_float_io_mpfr ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_float_io_mpfr PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_float_io_mpfr)
endif()

if(MPFI_COMPILED)
    cm_test(NAME multiprecision_test_test_float_io_mpfi SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp)
    target_compile_definitions(multiprecision_test_test_float_io_mpfi PUBLIC -DTEST_MPFI_50)
    target_link_libraries(multiprecision_test_test_float_io_mpfi ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES} ${TomMath_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_float_io_mpfi PRIVATE ${GMP_INCLUDE_DIRS} ${MPFI_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_float_io_mpfi)
endif()

if(FLOAT128_COMPILED)
    cm_test(NAME multiprecision_test_test_float_io_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp)
    target_compile_definitions(multiprecision_test_test_float_io_float128 PUBLIC -DTEST_FLOAT128)
    target_link_libraries(multiprecision_test_test_float_io_float128 ${QUADMATH_LIBRARIES} no_eh_support)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_float_io_float128)
endif()

if((INTEL_QUAD_COMPILED) AND (FLOAT128_COMPILED))
    cm_test(NAME multiprecision_test_test_float_io_intel_quad SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp)
    target_compile_definitions(multiprecision_test_test_float_io_intel_quad PUBLIC -DTEST_FLOAT128)
    target_link_libraries(multiprecision_test_test_float_io_intel_quad no_eh_support)
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_float_io_intel_quad)
endif()

if(TOMMATH_COMPILED)
    cm_test(NAME multiprecision_test_test_int_io_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_int_io.cpp)
    target_compile_definitions(multiprecision_test_test_int_io_tommath PUBLIC -DTEST_TOMMATH)
    target_link_libraries(multiprecision_test_test_int_io_tommath ${TomMath_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_int_io_tommath PRIVATE ${TomMath_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_int_io_tommath)
endif()

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_int_io_mpz SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_int_io.cpp)
    target_compile_definitions(multiprecision_test_test_int_io_mpz PUBLIC -DTEST_MPZ)
    target_link_libraries(multiprecision_test_test_int_io_mpz ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_int_io_mpz PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_conversions multiprecision_test_test_int_io_mpz)
endif()

cm_test(NAME multiprecision_test_test_int_io_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_int_io.cpp)
target_compile_definitions(multiprecision_test_test_int_io_cpp_int PUBLIC -DTEST_CPP_INT)
target_link_libraries(multiprecision_test_test_int_io_cpp_int no_eh_support)
add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_int_io_cpp_int)

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_cpp_int_left_shift SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_left_shift.cpp)
    target_link_libraries(multiprecision_test_test_cpp_int_left_shift ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_cpp_int_left_shift PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_left_shift)

    cm_test(NAME multiprecision_test_test_cpp_int_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int.cpp)
    target_compile_definitions(multiprecision_test_test_cpp_int_1 PUBLIC -DTEST1)
    target_link_libraries(multiprecision_test_test_cpp_int_1 ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_cpp_int_1 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_1)

    cm_test(NAME multiprecision_test_test_cpp_int_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int.cpp)
    target_compile_definitions(multiprecision_test_test_cpp_int_2 PUBLIC -DTEST2)
    target_link_libraries(multiprecision_test_test_cpp_int_2 ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_cpp_int_2 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_2)

    cm_test(NAME multiprecision_test_test_cpp_int_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int.cpp)
    target_compile_definitions(multiprecision_test_test_cpp_int_3 PUBLIC -DTEST3)
    target_link_libraries(multiprecision_test_test_cpp_int_3 ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_cpp_int_3 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_3)

    cm_test(NAME multiprecision_test_test_cpp_int_4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int.cpp)
    target_compile_definitions(multiprecision_test_test_cpp_int_4 PUBLIC -DTEST4)
    target_link_libraries(multiprecision_test_test_cpp_int_4 ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_cpp_int_4 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_4)

    cm_test(NAME multiprecision_test_test_cpp_int_5 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int.cpp)
    target_compile_definitions(multiprecision_test_test_cpp_int_5 PUBLIC -DTEST5)
    target_link_libraries(multiprecision_test_test_cpp_int_5 ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_cpp_int_5 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_5)
endif()

cm_test(NAME multiprecision_test_test_checked_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_checked_cpp_int.cpp)
target_link_libraries(multiprecision_test_test_checked_cpp_int no_eh_support)
cm_test(NAME multiprecision_test_test_unchecked_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_unchecked_cpp_int.cpp)
target_link_libraries(multiprecision_test_test_unchecked_cpp_int no_eh_support)
add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_unchecked_cpp_int)

cm_test(NAME multiprecision_test_test_cpp_int_serial_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_serial.cpp)
target_compile_definitions(multiprecision_test_test_cpp_int_serial_1 PUBLIC -DTEST1)
add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_serial_1)

cm_test(NAME multiprecision_test_test_cpp_int_serial_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_serial.cpp)
target_compile_definitions(multiprecision_test_test_cpp_int_serial_2 PUBLIC -DTEST2)
add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_serial_2)

cm_test(NAME multiprecision_test_test_cpp_int_serial_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_serial.cpp)
target_compile_definitions(multiprecision_test_test_cpp_int_serial_3 PUBLIC -DTEST3)
add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_serial_3)

cm_test(NAME multiprecision_test_test_cpp_int_serial_4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_serial.cpp)
target_compile_definitions(multiprecision_test_test_cpp_int_serial_4 PUBLIC -DTEST4)
add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_serial_4)

cm_test(NAME multiprecision_test_test_cpp_int_deserial SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_deserial.cpp)
add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_int_deserial)

cm_test(NAME multiprecision_test_test_cpp_rat_serial SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_rat_serial.cpp)
add_dependencies(multiprecision_test_suite_cpp_int_tests multiprecision_test_test_cpp_rat_serial)

if((FLOAT128_COMPILED) AND (QUADMATH_COMPILED))
    cm_test(NAME multiprecision_test_test_constexpr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_constexpr.cpp COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_test_constexpr PUBLIC -DHAVE_FLOAT128)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_constexpr)
endif()

cm_test(NAME multiprecision_test_test_nothrow_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_cpp_int.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_nothrow_cpp_int)

cm_test(NAME multiprecision_test_test_nothrow_cpp_rational SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_cpp_rational.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_nothrow_cpp_rational)

cm_test(NAME multiprecision_test_test_nothrow_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_cpp_bin_float.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_nothrow_cpp_bin_float)

cm_test(NAME multiprecision_test_test_nothrow_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_cpp_dec_float.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_nothrow_cpp_dec_float)

if(FLOAT128_COMPILED)
    cm_test(NAME multiprecision_test_test_nothrow_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_float128.cpp COMPILE_ONLY)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_nothrow_float128)
endif()

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_nothrow_gmp SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_gmp.cpp COMPILE_ONLY)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_nothrow_gmp)
endif()

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_nothrow_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_mpfr.cpp COMPILE_ONLY)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_nothrow_mpfr)
endif()

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_miller_rabin SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_miller_rabin.cpp)
    target_link_libraries(multiprecision_test_test_miller_rabin ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_miller_rabin PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_miller_rabin)
endif()

if(TOMMATH_COMPILED)
    cm_test(NAME multiprecision_test_test_rational_io_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rational_io.cpp)
    target_compile_definitions(multiprecision_test_test_rational_io_tommath PUBLIC -DTEST_TOMMATH)
    target_link_libraries(multiprecision_test_test_rational_io_tommath ${TomMath_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_rational_io_tommath PRIVATE ${TomMath_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rational_io_tommath)
endif()

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_rational_io_mpz SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rational_io.cpp)
    target_compile_definitions(multiprecision_test_test_rational_io_mpz PUBLIC -DTEST_MPQ)
    target_link_libraries(multiprecision_test_test_rational_io_mpz ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_rational_io_mpz PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rational_io_mpz)
endif()

cm_test(NAME multiprecision_test_test_rational_io_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rational_io.cpp)
target_compile_definitions(multiprecision_test_test_rational_io_cpp_int PUBLIC -DTEST_CPP_INT)
target_link_libraries(multiprecision_test_test_rational_io_cpp_int no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rational_io_cpp_int)

if((GMP_COMPILED) AND (TOMMATH_COMPILED) AND (MPFR_COMPILED))
    cm_test(NAME multiprecision_test_test_generic_conv SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_generic_conv.cpp)
    target_compile_definitions(multiprecision_test_test_generic_conv PUBLIC -DTEST_GMP -DTEST_TOMMATH -DTEST_MPFR)
    target_link_libraries(multiprecision_test_test_generic_conv ${TomMath_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_generic_conv PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_generic_conv)
endif()

cm_test(NAME multiprecision_test_test_rat_float_interconv_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
target_compile_definitions(multiprecision_test_test_rat_float_interconv_1 PUBLIC -DTEST1)
target_link_libraries(multiprecision_test_test_rat_float_interconv_1 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rat_float_interconv_1)

cm_test(NAME multiprecision_test_test_rat_float_interconv_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
target_compile_definitions(multiprecision_test_test_rat_float_interconv_2 PUBLIC -DTEST2)
target_link_libraries(multiprecision_test_test_rat_float_interconv_2 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rat_float_interconv_2)

cm_test(NAME multiprecision_test_test_rat_float_interconv_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
target_compile_definitions(multiprecision_test_test_rat_float_interconv_3 PUBLIC -DTEST3)
target_link_libraries(multiprecision_test_test_rat_float_interconv_3 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rat_float_interconv_3)

cm_test(NAME multiprecision_test_test_rat_float_interconv_4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
target_compile_definitions(multiprecision_test_test_rat_float_interconv_4 PUBLIC -DTEST4)
target_link_libraries(multiprecision_test_test_rat_float_interconv_4 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rat_float_interconv_4)

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_rat_float_interconv_5 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_5 PUBLIC -DTEST5)
    target_link_libraries(multiprecision_test_test_rat_float_interconv_5 ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_rat_float_interconv_5 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rat_float_interconv_5)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_6 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_6 PUBLIC -DTEST6)
    target_link_libraries(multiprecision_test_test_rat_float_interconv_6 ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_rat_float_interconv_6 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rat_float_interconv_6)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_7 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_7 PUBLIC -DTEST7)
    target_link_libraries(multiprecision_test_test_rat_float_interconv_7 ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_rat_float_interconv_7 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rat_float_interconv_7)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_8 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_8 PUBLIC -DTEST8)
    target_link_libraries(multiprecision_test_test_rat_float_interconv_8 ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_rat_float_interconv_8 PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_rat_float_interconv_8)
endif()

cm_test(NAME multiprecision_test_test_cpp_int_conv SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_conv.cpp)
target_link_libraries(multiprecision_test_test_cpp_int_conv no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_cpp_int_conv)

cm_test(NAME multiprecision_test_test_cpp_int_import_export SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_import_export.cpp)
target_link_libraries(multiprecision_test_test_cpp_int_import_export no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_cpp_int_import_export)

cm_test(NAME multiprecision_test_test_native_integer SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_native_integer.cpp)
target_link_libraries(multiprecision_test_test_native_integer no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_native_integer)

cm_test(NAME multiprecision_test_test_mixed_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_cpp_int.cpp)
target_link_libraries(multiprecision_test_test_mixed_cpp_int no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_mixed_cpp_int)

if((GMP_COMPILED) AND (MPFR_COMPILED))
    cm_test(NAME multiprecision_test_test_mixed_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_float.cpp)
    target_link_libraries(multiprecision_test_test_mixed_float ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_mixed_float)
endif()

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_include_test_mpfr_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/mpfr_include_test.cpp COMPILE_ONLY)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_include_test_mpfr_include_test)
endif()

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_include_test_gmp_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/gmp_include_test.cpp COMPILE_ONLY)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_include_test_gmp_include_test)
endif()

if(TOMMATH_COMPILED)
    cm_test(NAME multiprecision_test_include_test_tommath_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/tommath_include_test.cpp COMPILE_ONLY)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_include_test_tommath_include_test)
endif()

cm_test(NAME multiprecision_test_include_test_cpp_int_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/cpp_int_include_test.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_include_test_cpp_int_include_test)

cm_test(NAME multiprecision_test_include_test_cpp_dec_float_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/cpp_dec_float_include_test.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_include_test_cpp_dec_float_include_test)

cm_test(NAME multiprecision_test_ublas_interopinclude_test_cpp_bin_float_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/cpp_bin_float_include_test.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interopinclude_test_cpp_bin_float_include_test)

cm_test(NAME multiprecision_test_ublas_interop_test1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test1.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test11.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test12.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test13.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test1 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test1)

cm_test(NAME multiprecision_test_ublas_interop_test2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test2.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test21.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test22.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test23.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test2 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test2)

#run ublas_interop/test3.cpp ublas_interop/test31.cpp ublas_interop/test32.cpp ublas_interop/test33.cpp ;
cm_test(NAME multiprecision_test_ublas_interop_test4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test4.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test42.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test43.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test4 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test4)

cm_test(NAME multiprecision_test_ublas_interop_test5 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test5.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test52.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test53.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test5 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test5)

cm_test(NAME multiprecision_test_ublas_interop_test6 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test6.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test62.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test63.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test6 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test6)

#run ublas_interop/test7.cpp ublas_interop/test71.cpp ublas_interop/test72.cpp ublas_interop/test73.cpp ;

cm_test(NAME multiprecision_test_ublas_interop_test1_et SOURCES
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test1.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test11.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test12.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test13.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test1_et no_eh_support)
target_compile_definitions(multiprecision_test_ublas_interop_test1_et PUBLIC -DTEST_ET=1)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test1_et)

cm_test(NAME multiprecision_test_ublas_interop_test2_et SOURCES
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test2.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test21.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test22.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test23.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test2_et no_eh_support)
target_compile_definitions(multiprecision_test_ublas_interop_test2_et PUBLIC -DTEST_ET=1)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test2_et)

#run ublas_interop/test3.cpp ublas_interop/test31.cpp ublas_interop/test32.cpp ublas_interop/test33.cpp ;
cm_test(NAME multiprecision_test_ublas_interop_test4_et SOURCES
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test4.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test42.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test43.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test4_et no_eh_support)
target_compile_definitions(multiprecision_test_ublas_interop_test4_et PUBLIC -DTEST_ET=1)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test4_et)

cm_test(NAME multiprecision_test_ublas_interop_test5_et SOURCES
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test5.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test52.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test53.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test5_et no_eh_support)
target_compile_definitions(multiprecision_test_ublas_interop_test5_et PUBLIC -DTEST_ET=1)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test5_et)

cm_test(NAME multiprecision_test_ublas_interop_test6_et SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test6.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test62.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test63.cpp)
target_link_libraries(multiprecision_test_ublas_interop_test6_et no_eh_support)
target_compile_definitions(multiprecision_test_ublas_interop_test6_et PUBLIC -DTEST_ET=1)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_ublas_interop_test6_et)

#run ublas_interop/test7.cpp ublas_interop/test71.cpp ublas_interop/test72.cpp ublas_interop/test73.cpp ;

cm_test(NAME multiprecision_test_test_adapt_serial SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_adapt_serial.cpp)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_adapt_serial)

cm_test(NAME multiprecision_test_test_cpp_dec_float_serial_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_dec_float_serial.cpp)
target_compile_definitions(multiprecision_test_test_cpp_dec_float_serial_1 PUBLIC -DTEST1)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_cpp_dec_float_serial_1)

cm_test(NAME multiprecision_test_test_cpp_dec_float_serial_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_dec_float_serial.cpp)
target_compile_definitions(multiprecision_test_test_cpp_dec_float_serial_2 PUBLIC -DTEST2)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_cpp_dec_float_serial_2)

if(QUADMATH_COMPILED)
    cm_test(NAME multiprecision_test_test_float128_serial SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float128_serial.cpp)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_float128_serial)
endif()
#TODO: Enable this
#[[
cm_test(NAME multiprecision_test_test_bin_dec_float_serial_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_bin_float_serial.cpp)
target_compile_definitions(multiprecision_test_test_bin_dec_float_serial_1 PUBLIC -DTEST1)
cm_test(NAME multiprecision_test_test_bin_dec_float_serial_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_bin_float_serial.cpp)
target_compile_definitions(multiprecision_test_test_bin_dec_float_serial_2 PUBLIC -DTEST2)
]]
cm_test(NAME multiprecision_test_test_checked_mixed_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_checked_mixed_cpp_int.cpp)
target_link_libraries(multiprecision_test_test_checked_mixed_cpp_int no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_checked_mixed_cpp_int)

cm_test(NAME multiprecision_test_test_mixed_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_cpp_bin_float.cpp)
target_link_libraries(multiprecision_test_test_mixed_cpp_bin_float no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_checked_mixed_cpp_int)

cm_test(NAME multiprecision_test_test_mixed_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_cpp_dec_float.cpp)
target_link_libraries(multiprecision_test_test_mixed_cpp_dec_float no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_mixed_cpp_dec_float)

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_test_mixed_mpf_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_mpf_float.cpp)
    target_link_libraries(multiprecision_test_test_mixed_mpf_float ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_mixed_mpf_float PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_mixed_mpf_float)
endif()
if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_test_mixed_mpfr_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_mpfr_float.cpp)
    target_link_libraries(multiprecision_test_test_mixed_mpfr_float ${MPFR_LIBRARIES} ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_test_mixed_mpfr_float PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_mixed_mpfr_float)
endif()

cm_test(NAME multiprecision_test_test_float_conversions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_conversions.cpp)
target_link_libraries(multiprecision_test_test_float_conversions no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_float_conversions)

cm_test(NAME multiprecision_test_bug11922 SOURCES ${CURRENT_TEST_SOURCES_DIR}/bug11922.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_bug11922)

cm_test(NAME multiprecision_test_bug12039 SOURCES ${CURRENT_TEST_SOURCES_DIR}/bug12039.cpp)
target_link_libraries(multiprecision_test_bug12039 no_eh_support)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_bug12039)

cm_test(NAME multiprecision_test_git_issue_30 SOURCES ${CURRENT_TEST_SOURCES_DIR}/git_issue_30.cpp COMPILE_ONLY)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_git_issue_30)

if((GMP_COMPILED) AND (MPFR_COMPILED) AND (MPC_COMPILED) AND (FLOAT128_COMPILED))
    cm_test(NAME multiprecision_test_git_issue_98 ${CURRENT_TEST_SOURCES_DIR}/git_issue_98.cpp COMPILE_ONLY)
    target_link_libraries(multiprecision_test_git_issue_98 ${QUADMATH_LIBRARIES} ${MPC_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_compile_definitions(multiprecision_test_git_issue_98 PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPC -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_git_issue_98)
endif()

cm_test(NAME multiprecision_test_issue_13301 SOURCES ${CURRENT_TEST_SOURCES_DIR}/issue_13301.cpp)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_issue_13301)
cm_test(NAME multiprecision_test_issue_13148 SOURCES ${CURRENT_TEST_SOURCES_DIR}/issue_13148.cpp)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_issue_13148)

if((GMP_COMPILED) AND (MPFR_COMPILED) AND (MPFI_COMPILED) AND (TomMath_COMPILED) AND (FLOAT128_COMPILED))
    cm_test(NAME multiprecision_test_test_hash ${CURRENT_TEST_SOURCES_DIR}/test_hash.cpp COMPILE_ONLY)
    target_link_libraries(multiprecision_test_test_hash ${QUADMATH_LIBRARIES} ${MPFI_LIBRARIES} ${MPFR_LIBRARIES}
                          ${GMP_LIBRARIES} ${TomMath_LIBRARIES})
    target_compile_definitions(multiprecision_test_test_hash PRIVATE -DHAS_GMP -DHAS_MPFR -DHAS_MPFI -DHAS_TOMMATH -DFLOAT128_COMPILED)
    add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_hash)
endif()

cm_test(NAME multiprecision_test_test_optional_compat SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_optional_compat.cpp)
add_dependencies(multiprecision_test_suite_misc multiprecision_test_test_optional_compat)

# TODO: Fix this
#[[
if (EIGEN_COMPILED)
    cm_test(NAME multiprecision_test_test_eigen_interop_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_cpp_int.cpp)
    target_include_directories(multiprecision_test_test_eigen_interop_cpp_int PRIVATE ${EIGEN3_INCLUDE_DIRS})

    cm_test(NAME multiprecision_test_test_eigen_interop_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_cpp_dec_float.cpp)
    target_include_directories(multiprecision_test_test_eigen_interop_cpp_dec_float PRIVATE ${EIGEN3_INCLUDE_DIRS})

    cm_test(NAME multiprecision_test_test_eigen_interop_cpp_dec_float_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_cpp_dec_float_2.cpp)
    target_include_directories(multiprecision_test_test_eigen_interop_cpp_dec_float_2 PRIVATE ${EIGEN3_INCLUDE_DIRS})

    cm_test(NAME multiprecision_test_test_eigen_interop_cpp_dec_float_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_cpp_dec_float_3.cpp)
    target_include_directories(multiprecision_test_test_eigen_interop_cpp_dec_float_3 PRIVATE ${EIGEN3_INCLUDE_DIRS})

    cm_test(NAME multiprecision_test_test_eigen_interop_cpp_bin_float_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_cpp_bin_float_1.cpp)
    target_include_directories(multiprecision_test_test_eigen_interop_cpp_bin_float_1 PRIVATE ${EIGEN3_INCLUDE_DIRS})

    cm_test(NAME multiprecision_test_test_eigen_interop_cpp_bin_float_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_cpp_bin_float_2.cpp)
    target_include_directories(multiprecision_test_test_eigen_interop_cpp_bin_float_2 PRIVATE ${EIGEN3_INCLUDE_DIRS})

    cm_test(NAME multiprecision_test_test_eigen_interop_cpp_bin_float_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_cpp_bin_float_3.cpp)
    target_include_directories(multiprecision_test_test_eigen_interop_cpp_bin_float_3 PRIVATE  ${EIGEN3_INCLUDE_DIRS})
endif()

if ((EIGEN_COMPILED) AND (MPFR_COMPILED))
    cm_test(NAME multiprecision_test_test_eigen_interop_mpfr_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_mpfr_1.cpp)
    target_link_libraries(multiprecision_test_test_eigen_interop_mpfr_1 ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_eigen_interop_mpfr_1 PRIVATE ${EIGEN3_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS})

    cm_test(NAME multiprecision_test_test_eigen_interop_mpfr_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_mpfr_2.cpp)
    target_link_libraries(multiprecision_test_test_eigen_interop_mpfr_2 ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_eigen_interop_mpfr_2 PRIVATE ${EIGEN3_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS})

    cm_test(NAME multiprecision_test_test_eigen_interop_mpfr_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_mpfr_3.cpp)
    target_link_libraries(multiprecision_test_test_eigen_interop_mpfr_3 ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_eigen_interop_mpfr_3 PRIVATE ${EIGEN3_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS})
endif()

if ((EIGEN_COMPILED) AND (GMP_COMPILED))
    cm_test(NAME multiprecision_test_test_eigen_interop_gmp SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_gmp.cpp)
    target_link_libraries(multiprecision_test_test_eigen_interop_gmp ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_eigen_interop_gmp PRIVATE ${EIGEN3_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS})
endif()

if ((EIGEN_COMPILED) AND (MPC_COMPILED))
    cm_test(NAME multiprecision_test_test_eigen_interop_mpc SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_eigen_interop_mpc.cpp)
    target_link_libraries(multiprecision_test_test_eigen_interop_mpc ${MPC_LIBRARIES} ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
    target_include_directories(multiprecision_test_test_eigen_interop_mpc PUBLIC ${EIGEN3_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS})
endif()
#]]

#TODO: enable it
#[[
if (BUILD_WITH_SOURCES_DIR)
    file(GLOB TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/math/*.cpp)
    foreach(FILEB ${TEST_FILES})
        get_filename_component(NAME ${FILEB} NAME_WE)
        if(MPFR_COMPILED)
            cm_test(NAME multiprecision_test_math_${NAME}_mpfr SOURCES ${FILEB})
            target_compile_definitions(multiprecision_test_math_${NAME}_mpfr PRIVATE -DTEST_MPFR_50 -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
            target_include_directories(multiprecision_test_math_${NAME}_mpfr PRIVATE ${BUILD_WITH_SOURCES_DIR}/libs/math/include_private ${BUILD_WITH_SOURCES_DIR} ${GMP_INCLUDE_DIRS})
            target_link_libraries(multiprecision_test_math_${NAME}_mpfr ${MPFR_LIBRARIES} ${GMP_LIBRARIES})
        endif()

        if(GMP_COMPILED)
            cm_test(NAME multiprecision_test_math_${NAME}_mpf SOURCES ${FILEB})
            target_compile_definitions(multiprecision_test_math_${NAME}_mpf PRIVATE -DTEST_MPF_50 -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
            target_include_directories(multiprecision_test_math_${NAME}_mpf PRIVATE ${GMP_INCLUDE_DIRS} ${BUILD_WITH_SOURCES_DIR} ${BUILD_WITH_SOURCES_DIR}/libs/math/include_private)
            target_link_libraries(multiprecision_test_math_${NAME}_mpf ${GMP_LIBRARIES})
        endif()

        cm_test(NAME multiprecision_test_math_${NAME}_cpp_dec_float SOURCES ${FILEB})
        target_compile_definitions(multiprecision_test_math_${NAME}_cpp_dec_float PRIVATE -DTEST_CPP_DEC_FLOAT -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
        target_include_directories(multiprecision_test_math_${NAME}_cpp_dec_float PRIVATE ${BUILD_WITH_SOURCES_DIR} ${BUILD_WITH_SOURCES_DIR}/libs/math/include_private)

        cm_test(NAME multiprecision_test_math_${NAME}_cpp_bin_float SOURCES ${FILEB})
        target_compile_definitions(multiprecision_test_math_${NAME}_cpp_bin_float PRIVATE -DTEST_CPP_BIN_FLOAT -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
        target_include_directories(multiprecision_test_math_${NAME}_cpp_bin_float PRIVATE ${BUILD_WITH_SOURCES_DIR} ${BUILD_WITH_SOURCES_DIR}/libs/math/include_private)

        if(FLOAT128_COMPILED)
            cm_test(NAME multiprecision_test_math_${NAME}_float128 SOURCES ${FILEB})
            target_compile_definitions(multiprecision_test_math_${NAME}_float128 PRIVATE -DTEST_FLOAT128 -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
            target_include_directories(multiprecision_test_math_${NAME}_float128 PRIVATE ${BUILD_WITH_SOURCES_DIR} ${BUILD_WITH_SOURCES_DIR}/libs/math/include_private)
            target_link_libraries(multiprecision_test_math_${NAME}_float128 ${QUADMATH_LIBRARIES})
        endif()

        if((FLOAT128_COMPILED) AND (HAS_INTEL_QUAD))
            #TODO: fix test
            cm_test(NAME multiprecision_test_math_${NAME}_intel_quad SOURCES ${FILEB})
            target_compile_definitions(multiprecision_test_math_${NAME}_intel_quad PRIVATE -DTEST_FLOAT128 -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
            target_include_directories(multiprecision_test_math_${NAME}_intel_quad PRIVATE ${BUILD_WITH_SOURCES_DIR} ${BUILD_WITH_SOURCES_DIR}/libs/math/include_private)
        endif()
    endforeach()
endif()
]]

#TODO: implicit entry/start for main executable
#[[
if (BUILD_WITH_SOURCES_DIR)
    file(GLOB TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/math/high_prec/*.cpp)
    foreach(FILEB ${TEST_FILES})
        if(MPFR_COMPILED)
            get_filename_component(NAME ${FILEB} NAME_WE)
            cm_test(NAME multiprecision_test_math_high_prec_${NAME} SOURCES ${FILEB})
            target_compile_definitions(multiprecision_test_math_high_prec_${NAME} PRIVATE -DTEST_MPFR -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE -DBOOST_ALL_NO_LIB)
            target_include_directories(multiprecision_test_math_high_prec_${NAME} PRIVATE ${GMP_INCLUDE_DIRS} ${BUILD_WITH_SOURCES_DIR} ${BUILD_WITH_SOURCES_DIR}/libs/math/include_private)
            target_link_libraries(multiprecision_test_math_high_prec_${NAME} ${GMP_LIBRARIES} ${MPFR_LIBRARIES})
        endif()
    endforeach()
endif()
]]
file(GLOB TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/compile_fail/*.cpp)
foreach(FILEB ${TEST_FILES})
    if((GMP_COMPILED) AND (MPFR_COMPILED))
        get_filename_component(NAME ${FILEB} NAME_WE)
        cm_test(NAME multiprecision_test_compile_fail_${NAME} SOURCES ${FILEB} COMPILE_ONLY WILL_FAIL)
        target_compile_definitions(multiprecision_test_compile_fail_${NAME} PRIVATE -DTEST_GMP -DTEST_MPFR)
        add_dependencies(multiprecision_test_suite_compile_fail multiprecision_test_compile_fail_${NAME})
    endif()
endforeach()

if(BUILD_WITH_SOURCES_DIR)
    file(GLOB TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/concepts/*.cpp)
    foreach(FILEB ${TEST_FILES})
        get_filename_component(NAME ${FILEB} NAME_WE)

        if(MPFR_COMPILED)
            cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_50 SOURCES ${FILEB} COMPILE_ONLY)
            target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_50 PUBLIC -DTEST_MPFR_50)
            target_link_libraries(multiprecision_test_concepts_${NAME}_mpfr_50 ${MPFR_LIBRARIES})
            target_include_directories(multiprecision_test_concepts_${NAME}_mpfr_50 PRIVATE ${BUILD_WITH_SOURCES_DIR})
            add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_mpfr_50)

            cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_6 SOURCES ${FILEB} COMPILE_ONLY)
            target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_6 PUBLIC -DTEST_MPFR_6)
            target_link_libraries(multiprecision_test_concepts_${NAME}_mpfr_6 ${MPFR_LIBRARIES})
            target_include_directories(multiprecision_test_concepts_${NAME}_mpfr_6 PRIVATE ${BUILD_WITH_SOURCES_DIR})
            add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_mpfr_6)

            cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_15 SOURCES ${FILEB} COMPILE_ONLY)
            target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_15 PUBLIC -DTEST_MPFR_15)
            target_link_libraries(multiprecision_test_concepts_${NAME}_mpfr_15 ${MPFR_LIBRARIES})
            target_include_directories(multiprecision_test_concepts_${NAME}_mpfr_15 PRIVATE ${BUILD_WITH_SOURCES_DIR})
            add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_mpfr_15)

            cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_17 SOURCES ${FILEB} COMPILE_ONLY)
            target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_17 PUBLIC -DTEST_MPFR_17)
            target_link_libraries(multiprecision_test_concepts_${NAME}_mpfr_17 ${MPFR_LIBRARIES})
            target_include_directories(multiprecision_test_concepts_${NAME}_mpfr_17 PRIVATE ${BUILD_WITH_SOURCES_DIR})
            add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_mpfr_17)

            cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_30 SOURCES ${FILEB} COMPILE_ONLY)
            target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_30 PUBLIC -DTEST_MPFR_30)
            target_link_libraries(multiprecision_test_concepts_${NAME}_mpfr_30 ${MPFR_LIBRARIES})
            target_include_directories(multiprecision_test_concepts_${NAME}_mpfr_30 PRIVATE ${BUILD_WITH_SOURCES_DIR})
            add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_mpfr_30)
        endif()

        if(GMP_COMPILED)
            cm_test(NAME multiprecision_test_concepts_${NAME}_mpf50 SOURCES ${FILEB} COMPILE_ONLY)
            target_compile_definitions(multiprecision_test_concepts_${NAME}_mpf50 PUBLIC -DTEST_MPF_50)
            target_link_libraries(multiprecision_test_concepts_${NAME}_mpf50 ${GMP_LIBRARIES})
            target_include_directories(multiprecision_test_concepts_${NAME}_mpf50 PRIVATE ${BUILD_WITH_SOURCES_DIR})
            add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_mpf50)
        endif()

        cm_test(NAME multiprecision_test_concepts_${NAME}_cpp_dec_float SOURCES ${FILEB} COMPILE_ONLY)
        target_compile_definitions(multiprecision_test_concepts_${NAME}_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)
        target_include_directories(multiprecision_test_concepts_${NAME}_cpp_dec_float PRIVATE ${BUILD_WITH_SOURCES_DIR})
        add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_cpp_dec_float)

        cm_test(NAME multiprecision_test_concepts_${NAME}_cpp_bin_float SOURCES ${FILEB} COMPILE_ONLY)
        target_compile_definitions(multiprecision_test_concepts_${NAME}_cpp_bin_float PUBLIC -DTEST_CPP_BIN_FLOAT)
        target_include_directories(multiprecision_test_concepts_${NAME}_cpp_bin_float PRIVATE ${BUILD_WITH_SOURCES_DIR})
        add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_cpp_bin_float)

        cm_test(NAME multiprecision_test_concepts_${NAME}_cpp_dec_float_no_et SOURCES ${FILEB} COMPILE_ONLY)
        target_compile_definitions(multiprecision_test_concepts_${NAME}_cpp_dec_float_no_et PUBLIC -DTEST_CPP_DEC_FLOAT_NO_ET)
        target_include_directories(multiprecision_test_concepts_${NAME}_cpp_dec_float_no_et PRIVATE ${BUILD_WITH_SOURCES_DIR})
        add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_cpp_dec_float_no_et)

        cm_test(NAME multiprecision_test_concepts_${NAME}_backend_concept SOURCES ${FILEB} COMPILE_ONLY)
        target_compile_definitions(multiprecision_test_concepts_${NAME}_backend_concept PUBLIC -DTEST_BACKEND)
        target_include_directories(multiprecision_test_concepts_${NAME}_backend_concept PRIVATE ${BUILD_WITH_SOURCES_DIR})
        add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_backend_concept)

        cm_test(NAME multiprecision_test_concepts_${NAME}_logged_adaptor SOURCES ${FILEB} COMPILE_ONLY)
        target_compile_definitions(multiprecision_test_concepts_${NAME}_logged_adaptor PUBLIC -DTEST_LOGGED_ADAPTER)
        target_include_directories(multiprecision_test_concepts_${NAME}_logged_adaptor PRIVATE ${BUILD_WITH_SOURCES_DIR})
        add_dependencies(multiprecision_test_suite_concepts multiprecision_test_concepts_${NAME}_logged_adaptor)
    endforeach()
endif()

#test-suite examples

cm_test(NAME multiprecision_test_cpp_int_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/cpp_int_snips.cpp)
target_link_libraries(multiprecision_test_cpp_int_snips no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_cpp_int_snips)

cm_test(NAME multiprecision_test_cpp_int_import_export SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/cpp_int_import_export.cpp)
target_link_libraries(multiprecision_test_cpp_int_import_export no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_cpp_int_import_export)

cm_test(NAME multiprecision_test_cpp_bin_float_import_export SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/cpp_bin_float_import_export.cpp)
target_link_libraries(multiprecision_test_cpp_bin_float_import_export no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_cpp_bin_float_import_export)

cm_test(NAME multiprecision_test_cpp_dec_float_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/cpp_dec_float_snips.cpp)
target_link_libraries(multiprecision_test_cpp_dec_float_snips no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_cpp_dec_float_snips)

cm_test(NAME multiprecision_test_cpp_bin_float_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/cpp_bin_float_snips.cpp)
target_link_libraries(multiprecision_test_cpp_bin_float_snips no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_cpp_bin_float_snips)

cm_test(NAME multiprecision_test_debug_adaptor_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/debug_adaptor_snips.cpp)
target_link_libraries(multiprecision_test_debug_adaptor_snips no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_debug_adaptor_snips)

if(FLOAT128_COMPILED)
    #TODO: fix this
    #[ run float128_snips.cpp quadmath no_eh_eg_support : : : [ check-target-builds ../config//has_float128 : : <build>no ] ]
    #    cm_test(NAME multiprecision_test_debug_adaptor_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/debug_adaptor_snips.cpp)
    #    target_link_libraries(multiprecision_test_debug_adaptor_snips no_eh_support)
    #    add_dependencies(multiprecision_test_suite_examples multiprecision_test_debug_adaptor_snips)
endif()

cm_test(NAME multiprecision_test_floating_point_examples SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/floating_point_examples.cpp)
target_link_libraries(multiprecision_test_floating_point_examples no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_floating_point_examples)

#TODO: Enable this
#cm_test(NAME multiprecision_test_gauss_laguerre_quadrature SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/gauss_laguerre_quadrature.cpp)
#target_link_libraries(multiprecision_test_gauss_laguerre_quadrature no_eh_support)

#TODO: Fix this
#cm_test(NAME multiprecision_test_hypergeometric_luke_algorithms SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/hypergeometric_luke_algorithms.cpp)
#target_link_libraries(multiprecision_test_hypergeometric_luke_algorithms no_eh_support)

cm_test(NAME multiprecision_test_integer_examples SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/integer_examples.cpp)
target_link_libraries(multiprecision_test_integer_examples no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_integer_examples)

if(MPFI_COMPILED)
    cm_test(NAME multiprecision_test_logged_adaptor SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/logged_adaptor.cpp)
    target_link_libraries(multiprecision_test_logged_adaptor ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${MPFI_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_logged_adaptor PRIVATE ${GMP_INCLUDE_DIRS} ${MPFI_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_examples multiprecision_test_logged_adaptor)
endif()

cm_test(NAME multiprecision_test_mixed_integer_arithmetic SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/mixed_integer_arithmetic.cpp)
target_link_libraries(multiprecision_test_mixed_integer_arithmetic no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_mixed_integer_arithmetic)

if(FLOAT128_COMPILED)
    #TODO: fix this
    #[ run numeric_limits_snips.cpp no_eh_eg_support /boost//test_exec_monitor : : : [ requires cxx11_numeric_limits ]  [ check-target-builds ../config//has_float128 : <source>quadmath ] ]
endif()

if(GMP_COMPILED)
    #TODO: enable this
    #cm_test(NAME multiprecision_test_random_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/random_snips.cpp)
    #target_link_libraries(multiprecision_test_random_snips ${GMP_LIBRARIES} no_eh_support)
    #target_include_directories(multiprecision_test_random_snips PRIVATE ${GMP_INCLUDE_DIRS})
endif()

cm_test(NAME multiprecision_test_safe_prime SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/safe_prime.cpp)
target_link_libraries(multiprecision_test_safe_prime no_eh_support)
add_dependencies(multiprecision_test_suite_examples multiprecision_test_safe_prime)

if(GMP_COMPILED)
    cm_test(NAME multiprecision_test_gmp_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/gmp_snips.cpp)
    target_link_libraries(multiprecision_test_gmp_snips ${GMP_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_gmp_snips PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_examples multiprecision_test_gmp_snips)
endif()

if(MPFI_COMPILED)
    cm_test(NAME multiprecision_test_mpfi_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/mpfi_snips.cpp)
    target_link_libraries(multiprecision_test_mpfi_snips ${GMP_LIBRARIES} ${MPFI_LIBRARIES} ${MPFR_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_mpfi_snips PRIVATE ${GMP_INCLUDE_DIRS} ${MPFI_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_examples multiprecision_test_mpfi_snips)
endif()

if(MPFR_COMPILED)
    cm_test(NAME multiprecision_test_mpfr_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/mpfr_snips.cpp)
    target_link_libraries(multiprecision_test_mpfr_snips ${GMP_LIBRARIES} ${MPFR_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_mpfr_snips PRIVATE ${GMP_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_examples multiprecision_test_mpfr_snips)
endif()

if(TOMMATH_COMPILED)
    cm_test(NAME multiprecision_test_tommath_snips SOURCES ${CURRENT_TEST_SOURCES_DIR}/../example/tommath_snips.cpp)
    target_link_libraries(multiprecision_test_tommath_snips ${TomMath_LIBRARIES} no_eh_support)
    target_include_directories(multiprecision_test_tommath_snips PRIVATE ${TomMath_INCLUDE_DIRS})
    add_dependencies(multiprecision_test_suite_examples multiprecision_test_tommath_snips)
endif()

if(GMP_COMPILED AND TOMMATH_COMPILED)
    cm_test(NAME multiprecision_test_test_modular_adapter SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_modular_adapter.cpp)
    target_link_libraries(multiprecision_test_test_modular_adapter ${GMP_LIBRARIES} ${TomMath_LIBRARIES})
    target_include_directories(multiprecision_test_test_modular_adapter PRIVATE ${GMP_INCLUDE_DIRS})
    target_include_directories(multiprecision_test_test_modular_adapter PRIVATE ${TomMath_INCLUDE_DIRS})
endif()

include_directories(${CMAKE_WORKSPACE_SOURCES_DIR})

