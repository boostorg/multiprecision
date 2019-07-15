cmake_minimum_required(VERSION 3.5)
project(simple)

find_package(CM)

include(CMInstallTargets)
include(CMDeploy)
include(CMSetupVersion)
include(CMTest)
include(CMPkgConfig)

cm_setup_version(VERSION 1.0 GENERATE_HEADER simpleversion.hpp)

find_package(Threads)

add_library(simple INTERFACE)
target_include_directories(simple INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include2>)
target_compile_definitions(simple INTERFACE -DSOME_DEFINE=1 -DHAS_SIMPLE=1 -DDEFINE_3=3)
target_link_libraries(simple INTERFACE Threads::Threads)
if(NOT MSVC)
    target_compile_options(simple INTERFACE -std=c++0x)
endif()
cm_test_link_libraries(simple)
install(FILES test.cpp DESTINATION include2)
cm_deploy(TARGETS simple INCLUDE include)

cm_test(NAME simpletest SOURCES test.cpp)

cm_test(SOURCES test.cpp)

cm_test(NAME simpletestcompile SOURCES compiletest.cpp COMPILE_ONLY)

cm_test(SOURCES compiletest.cpp COMPILE_ONLY)

cm_test(NAME testwillfail SOURCES testwillfail.cpp WILL_FAIL)

cm_test(SOURCES testwillfail.cpp WILL_FAIL)

cm_test(NAME simpletestcompilewillfail SOURCES compiletestwillfail.cpp COMPILE_ONLY WILL_FAIL)

cm_test(SOURCES compiletestwillfail.cpp COMPILE_ONLY WILL_FAIL)

cm_test_header(NAME simpletestheader HEADER simple.h)

cm_test_header(HEADER simple.h)

cm_test_header(NAME simpleteststaticheader HEADER simple.h STATIC)

add_library(simpletestinterface INTERFACE)
cm_mark_as_test(simpletestinterface)

