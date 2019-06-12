include(CMTest)

if(BOOST_CMAKE)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_algorithm)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_chrono)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_exception)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_timer)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_numeric_ublas)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_filesystem)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_test)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_serialization)
    cm_find_package(${CMAKE_WORKSPACE_NAME}_numeric_interval)

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
                           )
else()
    cm_find_package(Boost COMPONENTS chrono timer exception filesystem
                    unit_test_framework serialization)

    cm_test_link_libraries(${CMAKE_WORKSPACE_NAME}_multiprecision
                           ${Boost_LIBRARIES}
                           )
endif()

# copyright John Maddock 2011
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt.

# HACK: Workaround broken includes
file(GLOB TEST_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/*.ipp)
foreach(HEADER ${TEST_HEADERS})
    configure_file(${HEADER} ${CURRENT_TEST_SOURCES_DIR}/include/libs/multiprecision/test/${HEADER} @ONLY)
endforeach()

add_library(multiprecision_test_settings INTERFACE)
target_include_directories(multiprecision_test_settings INTERFACE ${CURRENT_TEST_SOURCES_DIR}/include)
cm_test_link_libraries(multiprecision_test_settings)

# We set these to make it easier to set up and test GMP and MPFR under Win32:
# Speed up compiles:
# We can't yet enable this - it breaks the STL in some tests...
#<toolset>msvc,<optimization>off:<cxxflags>-RTCc
#<toolset>msvc,<optimization>off:<define>_ALLOW_RTCc_IN_STL

# add_library(no_eh_support STATIC no_eh_test_support.cpp)
# cm_mark_as_test(no_eh_support)
# target_link_libraries(no_eh_support boost_throw_exception)
# cm_target_link_test_libs(no_eh_support)

try_compile(HAS_GMP ${CURRENT_TEST_SOURCES_DIR}/../config ${CURRENT_TEST_SOURCES_DIR}/../config/has_gmp.cpp CMAKE_FLAGS
            "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'")
try_compile(HAS_MPFR ${CURRENT_TEST_SOURCES_DIR}/../config ${CURRENT_TEST_SOURCES_DIR}/../config/has_mpfr.cpp CMAKE_FLAGS
            "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'")
try_compile(HAS_TOMMATH ${CURRENT_TEST_SOURCES_DIR}/../config ${CURRENT_TEST_SOURCES_DIR}/../config/has_tommath.cpp CMAKE_FLAGS
            "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'")
try_compile(HAS_FLOAT128 ${CURRENT_TEST_SOURCES_DIR}/../config ${CURRENT_TEST_SOURCES_DIR}/../config/has_float128.cpp)
try_compile(HAS_MPFI ${CURRENT_TEST_SOURCES_DIR}/../config ${CURRENT_TEST_SOURCES_DIR}/../config/has_mpfi.cpp CMAKE_FLAGS
            "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'")
try_compile(HAS_INTEL_QUAD ${CURRENT_TEST_SOURCES_DIR}/../config ${CURRENT_TEST_SOURCES_DIR}/../config/has_intel_quad.cpp CMAKE_FLAGS
            "-DCMAKE_CXX_LINK_EXECUTABLE='echo not linking now...'")

cm_test(NAME multiprecision_test_test_arithmetic_backend_concept SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_backend_concept.cpp)

cm_test(NAME multiprecision_test_test_arithmetic_cpp_dec_float_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_dec_float_1.cpp)
cm_test(NAME multiprecision_test_test_arithmetic_cpp_dec_float_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_dec_float_2.cpp)
cm_test(NAME multiprecision_test_test_arithmetic_cpp_dec_float_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_dec_float_3.cpp)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_bin_float_1 SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_arithmetic_cpp_bin_float_1.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_bin_float_2 SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_arithmetic_cpp_bin_float_2.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_bin_float_3 SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_arithmetic_cpp_bin_float_3.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_arithmetic_mpf_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpf_50.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_mpf SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpf.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_mpz SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpz.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_mpz_rat SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpz_rat.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_mpz_br SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpz_br.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_mpq SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpq.cpp)
endif()

if(HAS_MPFR)
    cm_test(NAME multiprecision_test_test_arithmetic_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpfr.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_mpfr_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpfr_50.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_mpfr_50_static SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpfr_50_static.cpp)
endif()

if(HAS_TOMMATH)
    cm_test(NAME multiprecision_test_test_arithmetic_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_tommath.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_tommath_rat SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_tommath_rat.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_tommath_br SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_tommath_br.cpp)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_2.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_3.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_4.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_5 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_5.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_6 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_6.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_7 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_7.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_8 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_8.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_9 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_9.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_10 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_10.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_11 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_11.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_12 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_12.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_13 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_13.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_14 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_14.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_15 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_15.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_16 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_16.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_17 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_17.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_18 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_18.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_cpp_int_br SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_cpp_int_br.cpp)
endif()

cm_test(NAME multiprecision_test_test_arithmetic_ab_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_ab_1.cpp)
cm_test(NAME multiprecision_test_test_arithmetic_ab_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_ab_2.cpp)
cm_test(NAME multiprecision_test_test_arithmetic_ab_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_ab_3.cpp)

cm_test(NAME multiprecision_test_test_cpp_dec_float_round SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_dec_float_round.cpp)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_arithmetic_logged_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_logged_1.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_logged_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_logged_2.cpp)

    cm_test(NAME multiprecision_test_test_arithmetic_dbg_adptr1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_dbg_adptr1.cpp)
    cm_test(NAME multiprecision_test_test_arithmetic_dbg_adptr2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_dbg_adptr2.cpp)
endif()

if(HAS_MPFI)
    cm_test(NAME multiprecision_test_test_arithmetic_mpfi_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_arithmetic_mpfi_50.cpp)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_numeric_limits_backend_concept SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_backend_concept PUBLIC -DTEST_BACKEND)
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_numeric_limits_mpf50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpf50 PUBLIC -DTEST_MPF_50)

    cm_test(NAME multiprecision_test_test_numeric_limits_mpf SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpf PUBLIC -DTEST_MPF)

    cm_test(NAME multiprecision_test_test_numeric_limits_mpz SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpz PUBLIC -DTEST_MPZ)

    cm_test(NAME multiprecision_test_test_numeric_limits_mpq SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpq PUBLIC -DTEST_MPQ)
endif()

if(HAS_MPFR)
    cm_test(NAME multiprecision_test_test_numeric_limits_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpfr PUBLIC -DTEST_MPFR)

    cm_test(NAME multiprecision_test_test_numeric_limits_mpfr_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpfr_50 PUBLIC -DTEST_MPFR_50)
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_numeric_limits_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_cpp_dec_float PUBLIC -TDEST_CPP_DEC_FLOAT)

    cm_test(NAME multiprecision_test_test_numeric_limits_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_cpp_bin_float PUBLIC -DTEST_CPP_BIN_FLOAT)
endif()

if(HAS_TOMMATH)
    cm_test(NAME multiprecision_test_test_numeric_limits_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_tommath PUBLIC -DTEST_TOMMATH)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    cm_test(NAME multiprecision_test_test_numeric_limits_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_cpp_int PUBLIC -DTEST_CPP_INT)
endif()

if(HAS_MPFI)
    cm_test(NAME multiprecision_test_test_numeric_limits_mpfi_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_mpfi_50 PUBLIC -DTEST_MPFI_50)
endif()

if(HAS_FLOAT128)
    cm_test(NAME multiprecision_test_test_numeric_limits_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_float128 PUBLIC -DTEST_FLOAT128)
endif()

if((HAS_INTEL_QUAD) AND (HAS_FLOAT128))
    cm_test(NAME multiprecision_test_test_numeric_limits_intel_quad SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_numeric_limits.cpp)
    target_compile_definitions(multiprecision_test_test_numeric_limits_intel_quad PUBLIC -DTEST_FLOAT128)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(LOCAL_SOURCES "")
    # TODO: fix test. Reason error: [-fpermissive]
    set(LOCAL_SOURCES test_log.cpp test_pow.cpp test_sinh.cpp test_sqrt.cpp test_cosh.cpp test_tanh.cpp test_sin.cpp
        test_tan.cpp test_asin.cpp test_atan.cpp test_fpclassify.cpp)
    foreach(FILE ${LOCAL_SOURCES})
        get_filename_component(NAME ${FILE} NAME_WE)
        if(HAS_GMP)
            cm_test(NAME multiprecision_test_${NAME}_mpf50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
            target_compile_definitions(multiprecision_test_${NAME}_mpf50 PUBLIC -DTEST_MPF_50)
        endif()
        if(HAS_MPFR)
            cm_test(NAME multiprecision_test_${NAME}_mpfr50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
            target_compile_definitions(multiprecision_test_${NAME}_mpfr50 PUBLIC -DTEST_MPFR_50)
        endif()
        if(HAS_MPFI)
            cm_test(NAME multiprecision_test_${NAME}_mpfi50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
            target_compile_definitions(multiprecision_test_${NAME}_mpfi50 PUBLIC -DTEST_MPFI_50)
        endif()

        cm_test(NAME multiprecision_test_${FILE}_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
        target_compile_definitions(multiprecision_test_${FILE}_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)

        cm_test(NAME multiprecision_test_${NAME}_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
        target_compile_definitions(multiprecision_test_${NAME}_cpp_bin_float PUBLIC -DTEST_CPP_BIN_FLOAT)

        if(HAS_FLOAT128)
            cm_test(NAME multiprecision_test_${NAME}_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
            target_compile_definitions(multiprecision_test_${NAME}_float128 PUBLIC -DTEST_FLOAT128)
        endif()

        if(HAS_INTEL_QUAD)
            cm_test(NAME multiprecision_test_${NAME}_intel_quad SOURCES ${CURRENT_TEST_SOURCES_DIR}/${FILE})
            target_compile_definitions(multiprecision_test_${NAME}_intel_quad PUBLIC -DTEST_FLOAT128)
        endif()
    endforeach()
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_gmp_conversions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_gmp_conversions.cpp)
endif()

if(HAS_MPFR)
    cm_test(NAME multiprecision_test_test_mpfr_conversions SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mpfr_conversions.cpp)
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_constants_mpf50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_constants.cpp)
    target_compile_definitions(multiprecision_test_test_constants_mpf50 PUBLIC -DTEST_MPF_50)
endif()

if(HAS_MPFR)
    cm_test(NAME multiprecision_test_test_constants_mpfr_50 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_constants.cpp)
    target_compile_definitions(multiprecision_test_test_constants_mpfr_50 PUBLIC -DTEST_MPFR_50)
endif()

cm_test(NAME multiprecision_test_test_constants_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_constants.cpp)
target_compile_definitions(multiprecision_test_test_constants_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)

if(HAS_MPFR)
    cm_test(NAME multiprecision_test_test_move_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
    target_compile_definitions(multiprecision_test_test_move_mpfr PUBLIC -DTEST_MPFR)
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_move_gmp SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
    target_compile_definitions(multiprecision_test_test_move_gmp PUBLIC -DTEST_GMP)
endif()

if(HAS_TOMMATH)
    cm_test(NAME multiprecision_test_test_move_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
    target_compile_definitions(multiprecision_test_test_move_tommath PUBLIC -DTEST_TOMMATH)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_move_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_move.cpp)
    target_compile_definitions(multiprecision_test_test_move_cpp_int PUBLIC -DTEST_CPP_INT)
endif()

cm_test(NAME multiprecision_test_test_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_test.cpp)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_cpp_int_lit SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_lit.cpp)
endif()

if((HAS_FLOAT128) AND (HAS_INTEL_QUAD))
    cm_test(NAME multiprecision_test_test_constexpr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_constexpr.cpp COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_test_constexpr PUBLIC -DHAVE_FLOAT128)
endif()

cm_test(NAME multiprecision_test_test_nothrow_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_cpp_int.cpp COMPILE_ONLY)
cm_test(NAME multiprecision_test_test_nothrow_cpp_rational SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_cpp_rational.cpp COMPILE_ONLY)

cm_test(NAME multiprecision_test_test_nothrow_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_cpp_bin_float.cpp COMPILE_ONLY)
cm_test(NAME multiprecision_test_test_nothrow_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_cpp_dec_float.cpp COMPILE_ONLY)


if(HAS_FLOAT128)
    cm_test(NAME multiprecision_test_test_nothrow_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_float128.cpp COMPILE_ONLY)
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_nothrow_gmp SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_gmp.cpp COMPILE_ONLY)
endif()

if(HAS_MPFR)
    cm_test(NAME multiprecision_test_test_nothrow_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_nothrow_mpfr.cpp COMPILE_ONLY)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_cpp_bin_float_io_1 SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_cpp_bin_float_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    target_compile_definitions(multiprecision_test_test_cpp_bin_float_io_1 PUBLIC -DTEST1 -DTEST_CPP_BIN_FLOAT)

    cm_test(NAME multiprecision_test_test_cpp_bin_float_io_2 SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_cpp_bin_float_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    target_compile_definitions(multiprecision_test_test_cpp_bin_float_io_2 PUBLIC -DTEST2 -DTEST_CPP_BIN_FLOAT)

    if(HAS_MPFR)
        cm_test(NAME multiprecision_test_test_cpp_bin_float SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_cpp_bin_float.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_cpp_bin_float PUBLIC -DTEST_MPFR)
    endif()

    cm_test(NAME multiprecision_test_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    target_compile_definitions(multiprecision_test_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)

    if(HAS_GMP)
        cm_test(NAME multiprecision_test_test_float_io_mpf SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_float_io_mpf PUBLIC -DTEST_MPF_50)
    endif()

    if(HAS_MPFR)
        cm_test(NAME multiprecision_test_test_float_io_mpfr SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_float_io_mpfr PUBLIC -DTEST_MPFR_50)
    endif()

    if(HAS_MPFI)
        cm_test(NAME multiprecision_test_test_float_io_mpfi SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_float_io_mpfi PUBLIC -DTEST_MPFI_50)
    endif()

    if(HAS_FLOAT128)
        cm_test(NAME multiprecision_test_test_float_io_float128 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_float_io_float128 PUBLIC -DTEST_FLOAT128)
    endif()

    if((HAS_INTEL_QUAD) AND (HAS_FLOAT128))
        cm_test(NAME multiprecision_test_test_float_io_intel_quad SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_float_io_intel_quad PUBLIC -DTEST_FLOAT128)
    endif()

    if(HAS_TOMMATH)
        cm_test(NAME multiprecision_test_test_int_io_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_int_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_int_io_tommath PUBLIC -DTEST_TOMMATH)
    endif()

    if(HAS_GMP)
        cm_test(NAME multiprecision_test_test_int_io_mpz SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_int_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_int_io_mpz PUBLIC -DTEST_MPZ)
    endif()

    cm_test(NAME multiprecision_test_test_int_io_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_int_io.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    target_compile_definitions(multiprecision_test_test_int_io_cpp_int PUBLIC -DTEST_CPP_INT)

    if(HAS_GMP)
        cm_test(NAME multiprecision_test_test_cpp_int_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_cpp_int_1 PUBLIC -DTEST1)

        cm_test(NAME multiprecision_test_test_cpp_int_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_cpp_int_2 PUBLIC -DTEST2)

        cm_test(NAME multiprecision_test_test_cpp_int_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
        target_compile_definitions(multiprecision_test_test_cpp_int_3 PUBLIC -DTEST2)
    endif()
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_checked_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_checked_cpp_int.cpp)
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_miller_rabin SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_miller_rabin.cpp)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    if(HAS_TOMMATH)
        cm_test(NAME multiprecision_test_test_rational_io_tommath SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rational_io.cpp)
        target_compile_definitions(multiprecision_test_test_rational_io_tommath PUBLIC -DTEST_TOMMATH)
    endif()

    if(HAS_GMP)
        cm_test(NAME multiprecision_test_test_rational_io_mpz SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rational_io.cpp)
        target_compile_definitions(multiprecision_test_test_rational_io_mpz PUBLIC -DTEST_MPQ)
    endif()

    cm_test(NAME multiprecision_test_test_rational_io_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rational_io.cpp)
    target_compile_definitions(multiprecision_test_test_rational_io_cpp_int PUBLIC -DTEST_CPP_INT)
endif()

if((HAS_GMP) AND (HAS_TOMMATH) AND (HAS_MPFR))
    cm_test(NAME multiprecision_test_test_generic_conv SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_generic_conv.cpp)
    target_compile_definitions(multiprecision_test_test_generic_conv PUBLIC -DTEST_GMP -DTEST_TOMMATH -DTEST_MPFR)

endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_rat_float_interconv_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_1 PUBLIC TEST1)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_2 PUBLIC TEST2)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_3 PUBLIC TEST3)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_4 PUBLIC TEST4)
endif()

if(HAS_MPFR)
    cm_test(NAME multiprecision_test_test_rat_float_interconv_5 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_5 PUBLIC -DTEST5)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_6 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_6 PUBLIC -DTEST6)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_7 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_7 PUBLIC -DTEST7)

    cm_test(NAME multiprecision_test_test_rat_float_interconv_8 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_rat_float_interconv.cpp)
    target_compile_definitions(multiprecision_test_test_rat_float_interconv_8 PUBLIC -DTEST8)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_cpp_int_conv SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_conv.cpp)
    cm_test(NAME multiprecision_test_test_native_integer SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_native_integer.cpp)
    cm_test(NAME multiprecision_test_test_mixed_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_cpp_int.cpp)

    if((HAS_GMP) AND (HAS_MPFR))
        cm_test(NAME multiprecision_test_test_mixed_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_float.cpp)
    endif()
endif()

if(HAS_MPFR)
    cm_test(NAME multiprecision_test_include_test_mpfr_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/mpfr_include_test.cpp COMPILE_ONLY)
endif()
if(HAS_GMP)
    cm_test(NAME multiprecision_test_include_test_gmp_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/gmp_include_test.cpp COMPILE_ONLY)
endif()
if(HAS_TOMMATH)
    cm_test(NAME multiprecision_test_include_test_tommath_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/tommath_include_test.cpp COMPILE_ONLY)
endif()
cm_test(NAME multiprecision_test_include_test_cpp_int_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/cpp_int_include_test.cpp COMPILE_ONLY)
cm_test(NAME multiprecision_test_include_test_cpp_dec_float_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/cpp_dec_float_include_test.cpp COMPILE_ONLY)
cm_test(NAME multiprecision_test_ublas_interopinclude_test_cpp_bin_float_include_test SOURCES ${CURRENT_TEST_SOURCES_DIR}/include_test/cpp_bin_float_include_test.cpp COMPILE_ONLY)

cm_test(NAME multiprecision_test_ublas_interop_test1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test1.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test11.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test12.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test13.cpp)

cm_test(NAME multiprecision_test_ublas_interop_test2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test2.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test21.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test22.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test23.cpp)
#run ublas_interop/test3.cpp ublas_interop/test31.cpp ublas_interop/test32.cpp ublas_interop/test33.cpp ;
cm_test(NAME multiprecision_test_ublas_interop_test4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test4.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test42.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test43.cpp)
cm_test(NAME multiprecision_test_ublas_interop_test5 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test5.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test52.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test53.cpp)
cm_test(NAME multiprecision_test_ublas_interop_test6 SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test6.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test62.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test63.cpp)
#run ublas_interop/test7.cpp ublas_interop/test71.cpp ublas_interop/test72.cpp ublas_interop/test73.cpp ;

cm_test(NAME multiprecision_test_ublas_interop_test1_et SOURCES
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test1.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test11.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test12.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test13.cpp)
target_include_directories(multiprecision_test_ublas_interop_test1_et PUBLIC -DTEST_ET=1)
cm_test(NAME multiprecision_test_ublas_interop_test2_et SOURCES
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test2.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test21.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test22.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test23.cpp)
target_include_directories(multiprecision_test_ublas_interop_test2_et PUBLIC -DTEST_ET=1)
#run ublas_interop/test3.cpp ublas_interop/test31.cpp ublas_interop/test32.cpp ublas_interop/test33.cpp ;
cm_test(NAME multiprecision_test_ublas_interop_test4_et SOURCES
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test4.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test42.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test43.cpp)
target_include_directories(multiprecision_test_ublas_interop_test4_et PUBLIC -DTEST_ET=1)
cm_test(NAME multiprecision_test_ublas_interop_test5_et SOURCES
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test5.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test52.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test53.cpp)
target_include_directories(multiprecision_test_ublas_interop_test5_et PUBLIC -DTEST_ET=1)
cm_test(NAME multiprecision_test_ublas_interop_test6_et SOURCES ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test6.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test62.cpp
        ${CURRENT_TEST_SOURCES_DIR}/ublas_interop/test63.cpp)
target_include_directories(multiprecision_test_ublas_interop_test6_et PUBLIC -DTEST_ET=1)

#run ublas_interop/test7.cpp ublas_interop/test71.cpp ublas_interop/test72.cpp ublas_interop/test73.cpp ;
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_cpp_int_serial_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_serial.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    target_compile_definitions(multiprecision_test_test_cpp_int_serial_1 PUBLIC -DTEST1)
    cm_test(NAME multiprecision_test_test_cpp_int_serial_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_serial.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    target_compile_definitions(multiprecision_test_test_cpp_int_serial_2 PUBLIC -DTEST2)
    cm_test(NAME multiprecision_test_test_cpp_int_serial_3 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_serial.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    target_compile_definitions(multiprecision_test_test_cpp_int_serial_3 PUBLIC -DTEST3)
    cm_test(NAME multiprecision_test_test_cpp_int_serial_4 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_serial.cpp SOURCES_PREFIX ${CURRENT_TEST_SOURCES_DIR})
    target_compile_definitions(multiprecision_test_test_cpp_int_serial_4 PUBLIC -DTEST4)
    cm_test(NAME multiprecision_test_test_cpp_int_deserial SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_int_deserial.cpp)
    cm_test(NAME multiprecision_test_test_cpp_rat_serial SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_rat_serial.cpp)
    cm_test(NAME multiprecision_test_test_adapt_serial SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_adapt_serial.cpp)
    cm_test(NAME multiprecision_test_test_cpp_dec_float_serial_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_dec_float_serial.cpp)
    target_compile_definitions(multiprecision_test_test_cpp_dec_float_serial_1 PUBLIC -DTEST1)
    cm_test(NAME multiprecision_test_test_cpp_dec_float_serial_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_dec_float_serial.cpp)
    target_compile_definitions(multiprecision_test_test_cpp_dec_float_serial_2 PUBLIC -DTEST2)
endif()

if(HAS_FLOAT128)
    cm_test(NAME multiprecision_test_test_float128_serial SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_float128_serial.cpp)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # TODO: fix test. Reason error: [-fpermissive]
    cm_test(NAME multiprecision_test_test_bin_dec_float_serial_1 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_bin_float_serial.cpp)
    target_compile_definitions(multiprecision_test_test_bin_dec_float_serial_1 PUBLIC TEST1)

    cm_test(NAME multiprecision_test_test_bin_dec_float_serial_2 SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_cpp_bin_float_serial.cpp)
    target_compile_definitions(multiprecision_test_test_bin_dec_float_serial_2 PUBLIC TEST2)

    cm_test(NAME multiprecision_test_test_checked_mixed_cpp_int SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_checked_mixed_cpp_int.cpp)
    cm_test(NAME multiprecision_test_test_mixed_cpp_bin_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_cpp_bin_float.cpp)
    cm_test(NAME multiprecision_test_test_mixed_cpp_dec_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_cpp_dec_float.cpp)
endif()

if(HAS_GMP)
    cm_test(NAME multiprecision_test_test_mixed_mpf_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_mpf_float.cpp)
endif()
if(HAS_MPFR)
    cm_test(NAME multiprecision_test_test_mixed_mpfr_float SOURCES ${CURRENT_TEST_SOURCES_DIR}/test_mixed_mpfr_float.cpp)
endif()

file(GLOB TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/math/*.cpp)
# reason  error: right operand of shift expression ‘(1u << 63u)’ is >= than the precision of the left operand [-fpermissive]
set(TEST_FILES "")

foreach(FILEB ${TEST_FILES})
    get_filename_component(NAME ${FILEB} NAME_WE)
    if(HAS_MPFR)
        cm_test(NAME multiprecision_test_math_${NAME}_mpfr SOURCES ${FILEB})
        target_compile_definitions(multiprecision_test_math_${NAME}_mpfr PRIVATE -DTEST_MPFR_50 -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
        target_include_directories(multiprecision_test_math_${NAME}_mpfr PRIVATE ${CURRENT_TEST_SOURCES_DIR}/../../math/include_private)
    endif()

    if(HAS_GMP)
        cm_test(NAME multiprecision_test_math_${NAME}_mpf SOURCES ${FILEB})
        target_compile_definitions(multiprecision_test_math_${NAME}_mpf PRIVATE -DTEST_MPF_50 -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
        target_include_directories(multiprecision_test_math_${NAME}_mpf PRIVATE ${CURRENT_TEST_SOURCES_DIR}/../../math/include_private)
    endif()

    cm_test(NAME multiprecision_test_math_${NAME}_cpp_dec_float SOURCES ${FILEB})
    target_compile_definitions(multiprecision_test_math_${NAME}_cpp_dec_float PRIVATE -DTEST_CPP_DEC_FLOAT -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
    target_include_directories(multiprecision_test_math_${NAME}_cpp_dec_float PRIVATE ${CURRENT_TEST_SOURCES_DIR}/../../math/include_private)

    cm_test(NAME multiprecision_test_math_${NAME}_cpp_bin_float SOURCES ${FILEB})
    target_compile_definitions(multiprecision_test_math_${NAME}_cpp_bin_float PRIVATE -DTEST_CPP_BIN_FLOAT -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
    target_include_directories(multiprecision_test_math_${NAME}_cpp_bin_float PRIVATE ${CURRENT_TEST_SOURCES_DIR}/../../math/include_private)

    if(HAS_FLOAT128)
        cm_test(NAME multiprecision_test_math_${NAME}_float128 SOURCES ${FILEB})
        target_compile_definitions(multiprecision_test_math_${NAME}_float128 PRIVATE -DTEST_FLOAT128 -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
        target_include_directories(multiprecision_test_math_${NAME}_float128 PRIVATE ${CURRENT_TEST_SOURCES_DIR}/../../math/include_private)
    endif()

    if((HAS_FLOAT128) AND (HAS_INTEL_QUAD))
        cm_test(NAME multiprecision_test_math_${NAME}_intel_quad SOURCES ${FILEB})
        target_compile_definitions(multiprecision_test_math_${NAME}_intel_quad PRIVATE -DTEST_FLOAT128 -DBOOST_ALL_NO_LIB -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE)
        target_include_directories(multiprecision_test_math_${NAME}_intel_quad PRIVATE ${CURRENT_TEST_SOURCES_DIR}/../../math/include_private)
    endif()
endforeach()

file(GLOB TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/math/high_prec/*.cpp)
# reason  error: right operand of shift expression ‘(1u << 63u)’ is >= than the precision of the left operand [-fpermissive]
list(REMOVE_ITEM TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/math/high_prec/test_gamma.cpp)

foreach(FILEB ${TEST_FILES})
    get_filename_component(NAME ${FILEB} NAME_WE)
    cm_test(NAME multiprecision_test_math_high_prec_${NAME} SOURCES ${FILEB})
    if(HAS_MPFR)
        target_compile_definitions(multiprecision_test_math_high_prec_${NAME} PRIVATE -DTEST_MPFR)
    endif()
    target_compile_definitions(multiprecision_test_math_high_prec_${NAME} PRIVATE -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE -DBOOST_ALL_NO_LIB)
    target_include_directories(multiprecision_test_math_high_prec_${NAME} PRIVATE ${CURRENT_TEST_SOURCES_DIR}/../../math/include_private)
endforeach()

file(GLOB TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/compile_fail/*.cpp)
foreach(FILEB ${TEST_FILES})
    get_filename_component(NAME ${FILEB} NAME_WE)
    cm_test(NAME multiprecision_test_compile_fail_${NAME} SOURCES ${FILEB} COMPILE_ONLY WILL_FAIL)
    if(HAS_GMP)
        target_compile_definitions(multiprecision_test_compile_fail_${NAME} PUBLIC -DTEST_GMP)
    endif()
    if(HAS_MPFR)
        target_compile_definitions(multiprecision_test_compile_fail_${NAME} PUBLIC -DTEST_MPFR)
    endif()
endforeach()

file(GLOB TEST_FILES ${CURRENT_TEST_SOURCES_DIR}/concepts/*.cpp)
foreach(FILEB ${TEST_FILES})
    get_filename_component(NAME ${FILEB} NAME_WE)

    cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_50 SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_50 PUBLIC -DTEST_MPFR_50)

    cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_6 SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_6 PUBLIC -DTEST_MPFR_6)

    cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_15 SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_15 PUBLIC -DTEST_MPFR_15)

    cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_17 SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_17 PUBLIC -DTEST_MPFR_17)

    cm_test(NAME multiprecision_test_concepts_${NAME}_mpfr_30 SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_mpfr_30 PUBLIC -DTEST_MPFR_30)

    cm_test(NAME multiprecision_test_concepts_${NAME}_mpf50 SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_mpf50 PUBLIC -DTEST_MPF_50)

    cm_test(NAME multiprecision_test_concepts_${NAME}_cpp_dec_float SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_cpp_dec_float PUBLIC -DTEST_CPP_DEC_FLOAT)

    cm_test(NAME multiprecision_test_concepts_${NAME}_cpp_bin_float SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_cpp_bin_float PUBLIC -DTEST_CPP_BIN_FLOAT)

    cm_test(NAME multiprecision_test_concepts_${NAME}_cpp_dec_float_no_et SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_cpp_dec_float_no_et PUBLIC -DTEST_CPP_DEC_FLOAT_NO_ET)

    cm_test(NAME multiprecision_test_concepts_${NAME}_backend_concept SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_backend_concept PUBLIC -DTEST_BACKEND)

    cm_test(NAME multiprecision_test_concepts_${NAME}_logged_adaptor SOURCES ${FILEB} COMPILE_ONLY)
    target_compile_definitions(multiprecision_test_concepts_${NAME}_logged_adaptor PUBLIC -DTEST_LOGGED_ADAPTER)
endforeach()

include_directories(${CMAKE_WORKSPACE_SOURCES_DIR})