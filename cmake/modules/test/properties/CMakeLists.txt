cmake_minimum_required(VERSION 3.5)
project(properties)

find_package(CM)

include(CMProperties)
include(CMSetupVersion)
include(CMDeploy)
include(CMTest)

cm_setup_version(VERSION 1.0)

add_library(simple INTERFACE)

cm_install_targets(TARGETS simple INCLUDE include)
cm_auto_export(TARGETS simple)
cm_auto_pkgconfig(TARGET simple)

if(MSVC)
    add_compile_options(/we4541)
    add_compile_options(/we4530)
endif()

cm_test_link_libraries(simple)

cm_test(NAME simpletest SOURCES test.cpp)

cm_test(NAME rttitest SOURCES test.cpp)
set_target_properties(rttitest PROPERTIES CXX_RTTI FALSE)

cm_test(NAME exceptiontest SOURCES test.cpp)
set_target_properties(exceptiontest PROPERTIES CXX_EXCEPTIONS FALSE)

cm_test(NAME statictest SOURCES test.cpp)
set_target_properties(statictest PROPERTIES CXX_STATIC_RUNTIME TRUE)

cm_test(NAME throw SOURCES throw.cpp COMPILE_ONLY WILL_FAIL)
set_target_properties(throw PROPERTIES CXX_EXCEPTIONS FALSE)

cm_test(NAME throwpass SOURCES throw.cpp)
set_target_properties(throwpass PROPERTIES CXX_EXCEPTIONS TRUE)

cm_test(NAME rtti SOURCES rtti.cpp COMPILE_ONLY WILL_FAIL)
set_target_properties(rtti PROPERTIES CXX_RTTI FALSE)

cm_test(NAME rttipass SOURCES rtti.cpp)
set_target_properties(rttipass PROPERTIES CXX_RTTI TRUE)

foreach(WARNING_LEVEL FALSE TRUE all)
    cm_test(NAME warnings-${WARNING_LEVEL} SOURCES warnings.cpp)
    set_target_properties(warnings-${WARNING_LEVEL} PROPERTIES CXX_WARNINGS ${WARNING_LEVEL})

    if(WARNING_LEVEL STREQUAL "FALSE")
        cm_test(NAME warnings-errors-${WARNING_LEVEL} SOURCES warnings.cpp COMPILE_ONLY)
    else()
        cm_test(NAME warnings-errors-${WARNING_LEVEL} SOURCES warnings.cpp COMPILE_ONLY WILL_FAIL)
    endif()
    set_target_properties(warnings-errors-${WARNING_LEVEL} PROPERTIES CXX_WARNINGS ${WARNING_LEVEL})
    set_target_properties(warnings-errors-${WARNING_LEVEL} PROPERTIES CXX_WARNINGS_AS_ERRORS TRUE)

    cm_test(NAME nowarnings-${WARNING_LEVEL} SOURCES nowarnings.cpp)
    set_target_properties(nowarnings-${WARNING_LEVEL} PROPERTIES CXX_WARNINGS ${WARNING_LEVEL})

    cm_test(NAME nowarnings-errors-${WARNING_LEVEL} SOURCES nowarnings.cpp)
    set_target_properties(nowarnings-errors-${WARNING_LEVEL} PROPERTIES CXX_WARNINGS ${WARNING_LEVEL})
    set_target_properties(nowarnings-errors-${WARNING_LEVEL} PROPERTIES CXX_WARNINGS_AS_ERRORS TRUE)
endforeach()
