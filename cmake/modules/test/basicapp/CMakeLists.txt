cmake_minimum_required(VERSION 3.5)
project(basicapp)

find_package(CM)

include(CMInstallTargets)
include(CMDeploy)
include(CMTest)
include(CMSetupVersion)

cm_setup_version(VERSION 1.0)

find_package(simple)
add_executable(basicapp main.cpp)
target_link_libraries(basicapp simple)

cm_install_targets(TARGETS basicapp)

cm_test(NAME basciapptest SOURCES main.cpp)
target_link_libraries(basciapptest simple)

