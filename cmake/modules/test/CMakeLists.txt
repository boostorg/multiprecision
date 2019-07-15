
cmake_policy(SET CMP0057 NEW)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/tmp)

set(SKIP_TESTS)

if(DEFINED ENV{APPVEYOR})
    list(APPEND SKIP_TESTS
         pass-basic
         pass-simplecustomname
         pass-simplenamespace
         pass-simple-test
         )
endif()

function(create_test NAME TEST)
    set(CONTEXT)
    # get_cmake_property(VARS VARIABLES)
    foreach(VAR_NAME CMAKE_TOOLCHAIN_FILE CMAKE_CROSSCOMPILING)
        list(APPEND CONTEXT -D${VAR_NAME}=${${VAR_NAME}})
    endforeach()
    add_custom_target(${NAME} COMMAND ${CMAKE_COMMAND} ${CONTEXT} -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake ${TEST} ${CMAKE_CURRENT_BINARY_DIR}/tmp/${NAME})
    if(NAME IN_LIST SKIP_TESTS)
        add_test(NAME ${NAME} COMMAND echo skipped)
        set_tests_properties(${NAME} PROPERTIES DISABLED On)
    else()
        add_test(NAME ${NAME} COMMAND ${CMAKE_COMMAND} ${CONTEXT} -P ${CMAKE_CURRENT_SOURCE_DIR}/test.cmake ${TEST} ${CMAKE_CURRENT_BINARY_DIR}/tmp/${NAME})
    endif()
endfunction()

file(GLOB PASS_TESTS pass/*.cmake)
foreach(TEST ${PASS_TESTS})
    get_filename_component(NAME ${TEST} NAME_WE)
    create_test(pass-${NAME} ${TEST})
endforeach()

file(GLOB FAIL_TESTS fail/*.cmake)
foreach(TEST ${FAIL_TESTS})
    get_filename_component(NAME ${TEST} NAME_WE)
    create_test(fail-${NAME} ${TEST})
    set_tests_properties(fail-${NAME} PROPERTIES WILL_FAIL On)
endforeach()
