cmake_minimum_required(VERSION 3.5)
project(basic)

find_package(CM)

include(CMInstallTargets)
include(CMDeploy)
include(CMTest)
include(CMSetupVersion)

cm_setup_version(VERSION 1.0)

find_package(simple)
add_library(basic main.cpp)
target_link_libraries(basic lib::simple)

cm_deploy(TARGETS basic NAMESPACE lib::)

cm_test(NAME basictest SOURCES test.cpp)
target_link_libraries(basictest basic)

