
if(CMAKE_ARGC LESS 5)
    message(FATAL_ERROR "Not enough parameters to test.cmake")
endif()

math(EXPR TMP_ARGN "${CMAKE_ARGC}-1")
math(EXPR TEST_ARGN "${TMP_ARGN}-1")

string(RANDOM _TEST_RAND)
set(TEST ${CMAKE_ARGV${TEST_ARGN}})
set(TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
set(TMP_DIR ${CMAKE_ARGV${TMP_ARGN}}-${_TEST_RAND})
file(MAKE_DIRECTORY ${TMP_DIR})
set(PREFIX ${TMP_DIR}/usr)
set(BUILDS_DIR ${TMP_DIR}/builds)
# message("TMP_DIR: ${TMP_DIR}")

if(NOT EXISTS ${TEST})
    message(FATAL_ERROR "Test ${TEST} does not exist")
endif()

macro(test_expect_eq X Y)
    if(NOT ${X} EQUAL ${Y})
        message(FATAL_ERROR "EXPECT FAILURE: ${X} != ${Y}")
    endif()
endmacro()

macro(test_expect_file FILE)
    if(NOT EXISTS ${FILE})
        message(FATAL_ERROR "EXPECT FILE: ${FILE}")
    endif()
endmacro()

function(test_exec)
    string(REPLACE ";" " " OUTPUT "${ARGN}")
    message(${OUTPUT})
    execute_process(${ARGN} RESULT_VARIABLE RESULT)
    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "Process failed: ${OUTPUT}")
    endif()
endfunction()

function(install_dir DIR)
    set(options NO_INSTALL)
    set(oneValueArgs)
    set(multiValueArgs CMAKE_ARGS TARGETS)

    cmake_parse_arguments(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    string(RANDOM BUILD_RAND)
    set(BUILD_DIR ${BUILDS_DIR}/${BUILD_RAND})
    if(NOT EXISTS ${BUILD_DIR})
        file(MAKE_DIRECTORY ${BUILD_DIR})
    endif()
    if(CMAKE_TOOLCHAIN_FILE)
        set(TOOLCHAIN_ARG "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    endif()
    test_exec(COMMAND ${CMAKE_COMMAND}
              -DCMAKE_PREFIX_PATH=${PREFIX}
              -DCMAKE_INSTALL_PREFIX=${PREFIX}
              -DTHREADS_PREFER_PTHREAD_FLAG=1
              ${TOOLCHAIN_ARG}
              ${PARSE_CMAKE_ARGS}
              ${DIR}
              WORKING_DIRECTORY ${BUILD_DIR}
              )
    test_exec(COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR})
    foreach(TARGET ${PARSE_TARGETS})
        test_exec(COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR} --target ${TARGET})
    endforeach()
    if(NOT PARSE_NO_INSTALL)
        test_exec(COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR} --target install)
    endif()

    file(REMOVE_RECURSE ${BUILD_DIR})
endfunction()

function(build_dir DIR)
    install_dir(${DIR} ${ARGN} NO_INSTALL)
endfunction()

function(test_check_pkgconfig)
    set(options)
    set(oneValueArgs NAME HEADER)
    set(multiValueArgs)

    cmake_parse_arguments(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(HEADER_FLAG)
    if(PARSE_HEADER)
        set(HEADER_FLAG -DPKG_CONFIG_HEADER=${PARSE_HEADER})
    endif()

    # TODO: We really should check pkgconfig during cross compiling
    if(NOT CMAKE_CROSSCOMPILING)
        install_dir(${TEST_DIR}/pkgconfigcheck TARGETS check CMAKE_ARGS -DPKG_CONFIG_MODULES=${PARSE_NAME} ${HEADER_FLAG})
    endif()
endfunction()

function(test_check_package)
    set(options)
    set(oneValueArgs NAME HEADER TARGET)
    set(multiValueArgs)

    cmake_parse_arguments(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(HEADER_FLAG)
    if(PARSE_HEADER)
        set(HEADER_FLAG -DPKG_HEADER=${PARSE_HEADER})
    endif()

    set(TARGET ${PARSE_NAME})
    if(PARSE_TARGET)
        set(TARGET ${PARSE_TARGET})
    endif()

    install_dir(${TEST_DIR}/findpackagecheck CMAKE_ARGS -DPKG=${PARSE_NAME} -DPKG_TARGET=${TARGET} ${HEADER_FLAG})
endfunction()

install_dir(${TEST_DIR}/../)

include(${TEST})

file(REMOVE_RECURSE ${TMP_DIR})
