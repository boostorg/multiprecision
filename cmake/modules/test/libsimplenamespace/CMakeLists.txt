cmake_minimum_required(VERSION 3.5)
project(simple)

find_package(CM)

include(CMInstallTargets)
include(CMDeploy)
include(CMSetupVersion)
include(CMTest)

cm_setup_version(VERSION 1.0)

add_library(simple INTERFACE)
target_compile_definitions(simple INTERFACE -DHAS_SIMPLE=1)
cm_deploy(TARGETS simple INCLUDE include NAMESPACE lib::)

cm_test(NAME simpletest SOURCES test.cpp)
target_link_libraries(simpletest lib::simple)

cm_test(NAME simpletestcompile SOURCES compiletest.cpp COMPILE_ONLY)
target_link_libraries(simpletestcompile lib::simple)

cm_test_header(NAME simpletestheader HEADER simple.h)
target_link_libraries(simpletestheader lib::simple)

cm_test_header(NAME simpleteststaticheader HEADER simple.h STATIC)
target_link_libraries(simpleteststaticheader lib::simple)

