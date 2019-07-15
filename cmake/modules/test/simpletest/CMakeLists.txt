cmake_minimum_required(VERSION 3.5)
project(simple)

find_package(CM)

include(CMInstallTargets)
include(CMDeploy)
include(CMTest)
include(CMSetupVersion)

cm_setup_version(VERSION 1.0)

add_library(simple simple.cpp)
target_include_directories(simple PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

cm_deploy(TARGETS simple INCLUDE include)

cm_test(NAME simpletoptest SOURCES test.cpp)
target_link_libraries(simpletoptest simple)

cm_add_test_subdirectory(test)

if(TARGET simpletest)
    message(FATAL_ERROR "simpletest was not added")
endif()

