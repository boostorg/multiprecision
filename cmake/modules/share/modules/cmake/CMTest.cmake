option(BUILD_TESTS "Controls whether to build the tests as part of the main build" FALSE)

enable_testing()

foreach (scope GLOBAL DIRECTORY)
    define_property(${scope} PROPERTY "ENABLE_TESTS" INHERITED
            BRIEF_DOCS "Enable tests"
            FULL_DOCS "Enable tests"
            )
endforeach ()
option(CMAKE_ENABLE_TESTS "Enable tests" ON)
set_property(GLOBAL PROPERTY ENABLE_TESTS ${CMAKE_ENABLE_TESTS})

include(ProcessorCount)
processorcount(_cm_ctest_parallel_level)
set(CTEST_PARALLEL_LEVEL ${_cm_ctest_parallel_level} CACHE STRING "CTest parallel level")

if (NOT TARGET check)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C ${CMAKE_CFG_INTDIR} -j ${CTEST_PARALLEL_LEVEL} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()


if (NOT TARGET tests)
    add_custom_target(tests COMMENT "Build all tests.")
    add_dependencies(check tests)
endif ()

if (NOT TARGET check-${PROJECT_NAME})
    add_custom_target(check-${PROJECT_NAME} COMMAND ${CMAKE_CTEST_COMMAND} -L ${PROJECT_NAME} --output-on-failure -C ${CMAKE_CFG_INTDIR} -j ${CTEST_PARALLEL_LEVEL} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()

if (NOT TARGET tests-${PROJECT_NAME})
    add_custom_target(tests-${PROJECT_NAME} COMMENT "Build all tests for ${PROJECT_NAME}.")
    add_dependencies(check-${PROJECT_NAME} tests-${PROJECT_NAME})
endif ()

function(cm_mark_as_test)
    foreach (TEST_TARGET ${ARGN})
        if (NOT BUILD_TESTS)
            get_target_property(TEST_TARGET_TYPE ${TEST_TARGET} TYPE)
            # We can onle use EXCLUDE_FROM_ALL on build targets
            if (NOT "${TEST_TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
                set_target_properties(${TEST_TARGET}
                        PROPERTIES EXCLUDE_FROM_ALL TRUE
                        )
            endif ()
        endif ()
        add_dependencies(tests ${TEST_TARGET})
        add_dependencies(tests-${PROJECT_NAME} ${TEST_TARGET})
    endforeach ()
endfunction(cm_mark_as_test)


function(cm_create_internal_targets)
    if (NOT TARGET _cm_internal_tests-${PROJECT_NAME})
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/_cm_internal_tests-${PROJECT_NAME}.cpp "")
        add_library(_cm_internal_tests-${PROJECT_NAME} STATIC ${CMAKE_CURRENT_BINARY_DIR}/_cm_internal_tests-${PROJECT_NAME}.cpp)
        cm_mark_as_test(_cm_internal_tests-${PROJECT_NAME})
    endif ()
endfunction()

foreach (scope DIRECTORY TARGET)
    define_property(${scope} PROPERTY "CM_TEST_DEPENDENCIES" INHERITED
            BRIEF_DOCS "Default test dependencies"
            FULL_DOCS "Default test dependencies"
            )
endforeach ()

function(cm_test_link_libraries)
    cm_create_internal_targets()
    if (BUILD_TESTS)
        set_property(DIRECTORY APPEND PROPERTY CM_TEST_DEPENDENCIES ${ARGN})
        target_link_libraries(_cm_internal_tests-${PROJECT_NAME} ${ARGN})
    else ()
        foreach (TARGET ${ARGN})
            if (TARGET ${TARGET})
                set_property(DIRECTORY APPEND PROPERTY CM_TEST_DEPENDENCIES ${TARGET})
                target_link_libraries(_cm_internal_tests-${PROJECT_NAME} ${TARGET})
            elseif (${TARGET} MATCHES "::")
                cm_shadow_exists(HAS_TARGET ${TARGET})
                set_property(DIRECTORY APPEND PROPERTY CM_TEST_DEPENDENCIES $<${HAS_TARGET}:${TARGET}>)
                target_link_libraries(_cm_internal_tests-${PROJECT_NAME} $<${HAS_TARGET}:${TARGET}>)
            else ()
                set_property(DIRECTORY APPEND PROPERTY CM_TEST_DEPENDENCIES ${TARGET})
                target_link_libraries(_cm_internal_tests-${PROJECT_NAME} ${TARGET})
            endif ()
            if (BUILD_SHARED_LIBS)
                target_compile_definitions(_cm_internal_tests-${PROJECT_NAME} PRIVATE -DBOOST_TEST_DYN_LINK=1 -DBOOST_TEST_NO_AUTO_LINK=1)
            endif ()
        endforeach ()
    endif ()
endfunction()

function(cm_target_link_test_libs TARGET)
    # target_link_libraries(${TARGET}
    #     $<TARGET_PROPERTY:CM_TEST_DEPENDENCIES>
    # )
    get_property(DEPS DIRECTORY PROPERTY CM_TEST_DEPENDENCIES)
    target_link_libraries(${TARGET} ${DEPS})
endfunction()


function(cm_test)
    set(options COMPILE_ONLY WILL_FAIL NO_TEST_LIBS)
    set(oneValueArgs NAME SOURCES_PREFIX)
    set(multiValueArgs SOURCES CONTENT ARGS)

    cmake_parse_arguments(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (PARSE_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to cm_test(): \"${PARSE_UNPARSED_ARGUMENTS}\"")
    endif ()

    set(SOURCES ${PARSE_SOURCES})

    if (PARSE_NAME)
        set(TEST_NAME ${PARSE_NAME})
    else ()
        string(MAKE_C_IDENTIFIER "${PROJECT_NAME}_${SOURCES}_test" TEST_NAME)
    endif ()

    if (PARSE_CONTENT)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generated-${TEST_NAME}.cpp "${PARSE_CONTENT}")
        set(SOURCES ${CMAKE_CURRENT_BINARY_DIR}/generated-${TEST_NAME}.cpp)
    endif ()

    if (PARSE_COMPILE_ONLY)
        add_library(${TEST_NAME} STATIC EXCLUDE_FROM_ALL ${SOURCES})
        add_test(NAME ${TEST_NAME}
                COMMAND ${CMAKE_COMMAND} --build . --target ${TEST_NAME} --config $<CONFIGURATION>
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        if (PARSE_SOURCES_PREFIX)
            target_include_directories(${TEST_NAME} PRIVATE ${PARSE_SOURCES_PREFIX})
        else ()
            target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
        endif ()

        # set_tests_properties(${TEST_NAME} PROPERTIES RESOURCE_LOCK cm_test_compile_only)
    else ()
        add_executable(${TEST_NAME} ${SOURCES})
        cm_mark_as_test(${TEST_NAME})
        if (PARSE_SOURCES_PREFIX)
            target_include_directories(${TEST_NAME} PRIVATE ${PARSE_SOURCES_PREFIX})
        else ()
            target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
        endif ()
        if (WIN32)
            foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES} "")
                file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}-test-run-${CONFIG}.cmake CONTENT "
include(\"${CMAKE_BINARY_DIR}/cm_set_rpath-$<CONFIG>.cmake\")
if(CMAKE_CROSSCOMPILING)
foreach(RP \${RPATH})
    execute_process(COMMAND winepath -w \${RP} OUTPUT_VARIABLE _RPATH)
    string(STRIP \"\${_RPATH}\" _RPATH)
    set(ENV{WINEPATH} \"\${_RPATH};\$ENV{WINEPATH}\")
endforeach()
else()
set(ENV{PATH} \"\${RPATH};\$ENV{PATH}\")
endif()
execute_process(
    COMMAND $<TARGET_FILE:${TEST_NAME}> ${PARSE_ARGS} 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
    RESULT_VARIABLE RESULT)
if(NOT RESULT EQUAL 0)
    message(FATAL_ERROR \"Test failed\")
endif()
" CONDITION $<CONFIG:${CONFIG}>)
            endforeach ()
            add_test(NAME ${TEST_NAME} COMMAND ${CMAKE_COMMAND} -DCMAKE_CROSSCOMPILING=${CMAKE_CROSSCOMPILING} -P ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}-test-run-$<CONFIG>.cmake)
        else ()
            add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} ${PARSE_ARGS} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        endif ()
    endif ()

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${TEST_NAME} PRIVATE -DBOOST_TEST_DYN_LINK=1 -DBOOST_TEST_NO_AUTO_LINK=1)
    endif ()

    if (PARSE_WILL_FAIL)
        set_tests_properties(${TEST_NAME} PROPERTIES WILL_FAIL TRUE)
    endif ()
    set_tests_properties(${TEST_NAME} PROPERTIES LABELS ${PROJECT_NAME})
    if (NOT PARSE_NO_TEST_LIBS)
        cm_target_link_test_libs(${TEST_NAME})
    endif ()
endfunction(cm_test)

function(cm_test_header)
    set(options STATIC NO_TEST_LIBS)
    set(oneValueArgs NAME HEADER)
    set(multiValueArgs)

    cmake_parse_arguments(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (PARSE_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown keywords given to cm_test_header(): \"${PARSE_UNPARSED_ARGUMENTS}\"")
    endif ()

    if (PARSE_NAME)
        set(TEST_NAME ${PARSE_NAME})
    else ()
        string(MAKE_C_IDENTIFIER "${PROJECT_NAME}_${PARSE_HEADER}_header_test" TEST_NAME)
    endif ()

    if (PARSE_STATIC)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/header-main-include-${TEST_NAME}.cpp
                "#include <${PARSE_HEADER}>\nint main() {}\n"
                )
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/header-static-include-${TEST_NAME}.cpp
                "#include <${PARSE_HEADER}>\n"
                )
        cm_test(NAME ${TEST_NAME} SOURCES
                ${CMAKE_CURRENT_BINARY_DIR}/header-main-include-${TEST_NAME}.cpp
                ${CMAKE_CURRENT_BINARY_DIR}/header-static-include-${TEST_NAME}.cpp
                )
    else ()
        cm_test(NAME ${TEST_NAME} CONTENT
                "#include <${PARSE_HEADER}>\nint main() {}\n"
                )
    endif ()
    set_tests_properties(${TEST_NAME} PROPERTIES LABELS ${PROJECT_NAME})
endfunction(cm_test_header)

macro(cm_add_test_subdirectory)
    get_directory_property(_enable_tests_property ENABLE_TESTS)
    get_property(_enable_tests_global_property GLOBAL PROPERTY ENABLE_TESTS)
    string(TOUPPER "${_enable_tests_property}" _enable_tests_property_upper)
    if (_enable_tests_property_upper STREQUAL "OFF" OR _enable_tests_property_upper EQUAL 1)
        set(CURRENT_SOURCES_DIR ${CURRENT_SOURCES_DIR}/test)
        add_subdirectory(${ARGN})
    endif ()
endmacro()
