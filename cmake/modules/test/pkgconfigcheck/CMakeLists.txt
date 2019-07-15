cmake_minimum_required(VERSION 3.5)
project(pkgconfigcheck)

find_package(CM)

include(CMInstallTargets)
include(CMDeploy)
include(CMSetupVersion)
include(CMTest)
include(CMPkgConfig)

set(_pkgconfig_path)
foreach(PATH ${CMAKE_PREFIX_PATH})
    list(APPEND _pkgconfig_path ${PATH}/lib/pkgconfig)
    list(APPEND _pkgconfig_path ${PATH}/share/pkgconfig)
endforeach()

if(NOT "${_pkgconfig_path}" STREQUAL "")
    # remove empty values from the list
    list(REMOVE_ITEM _pkgconfig_path "")
    file(TO_NATIVE_PATH "${_pkgconfig_path}" _pkgconfig_path)
    if(UNIX)
        string(REPLACE ";" ":" _pkgconfig_path "${_pkgconfig_path}")
        string(REPLACE "\\ " " " _pkgconfig_path "${_pkgconfig_path}")
    endif()
    set(ENV{PKG_CONFIG_PATH} "${_pkgconfig_path}")
endif()

find_package(PkgConfig)
pkg_check_modules(PKGS REQUIRED ${PKG_CONFIG_MODULES})

cm_setup_version(VERSION 1.0 GENERATE_HEADER pkgconfigcheckversion.hpp)

add_library(pkgconfigcheck INTERFACE)
target_compile_options(pkgconfigcheck INTERFACE ${PKGS_CFLAGS})
target_link_libraries(pkgconfigcheck INTERFACE ${PKGS_LDFLAGS})
cm_test_link_libraries(pkgconfigcheck)

cm_install_targets(TARGETS pkgconfigcheck)
cm_auto_export(TARGETS pkgconfigcheck)

if(DEFINED PKG_CONFIG_HEADER)
    cm_test_header(NAME testheader HEADER ${PKG_CONFIG_HEADER})
    cm_test_header(NAME teststaticheader HEADER ${PKG_CONFIG_HEADER} STATIC)
endif()

