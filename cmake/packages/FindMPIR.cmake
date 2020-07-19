#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#
#
# Try to find the MPIR library
# https://mpir.org/
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(MPIR 6.0.0)
# to require version 6.0.0 to newer of MPIR.
#
# Once done this will define
#
#  MPIR_FOUND - system has MPIR lib with correct version
#  MPIR_INCLUDE_DIRS - the MPIR include directory
#  MPIR_LIBRARIES - the MPIR library
#  MPIR_VERSION - MPIR version

find_path(MPIR_INCLUDE_DIRS NAMES gmp.h PATHS $ENV{MPIRDIR} ${INCLUDE_INSTALL_DIR})

# Set MPIR_FIND_VERSION to 5.1.0 if no minimum version is specified
if(NOT MPIR_FIND_VERSION)
    if(NOT MPIR_FIND_VERSION_MAJOR)
        set(MPIR_FIND_VERSION_MAJOR 5)
    endif()
    if(NOT MPIR_FIND_VERSION_MINOR)
        set(MPIR_FIND_VERSION_MINOR 1)
    endif()
    if(NOT MPIR_FIND_VERSION_PATCH)
        set(MPIR_FIND_VERSION_PATCH 0)
    endif()
    set(MPIR_FIND_VERSION
        "${MPIR_FIND_VERSION_MAJOR}.${MPIR_FIND_VERSION_MINOR}.${MPIR_FIND_VERSION_PATCH}")
endif()

if(MPIR_INCLUDE_DIRS)
    # Since the MPIR version macros may be in a file included by gmp.h of the form
    # gmp-.*[_]?.*.h (e.g., gmp-x86_64.h), we search each of them.
    file(GLOB MPIR_HEADERS "${MPIR_INCLUDE_DIRS}/gmp.h" "${MPIR_INCLUDE_DIRS}/gmp-*.h")
    foreach(gmp_header_filename ${MPIR_HEADERS})
        file(READ "${gmp_header_filename}" _gmp_version_header)
        string(REGEX MATCH
               "define[ \t]+__GNU_MP_VERSION[ \t]+([0-9]+)" _gmp_major_version_match
               "${_gmp_version_header}")
        if(_gmp_major_version_match)
            set(MPIR_MAJOR_VERSION "${CMAKE_MATCH_1}")
            string(REGEX MATCH "define[ \t]+__GNU_MP_VERSION_MINOR[ \t]+([0-9]+)"
                   _gmp_minor_version_match "${_gmp_version_header}")
            set(MPIR_MINOR_VERSION "${CMAKE_MATCH_1}")
            string(REGEX MATCH "define[ \t]+__GNU_MP_VERSION_PATCHLEVEL[ \t]+([0-9]+)"
                   _gmp_patchlevel_version_match "${_gmp_version_header}")
            set(MPIR_PATCHLEVEL_VERSION "${CMAKE_MATCH_1}")
            set(MPIR_VERSION
                ${MPIR_MAJOR_VERSION}.${MPIR_MINOR_VERSION}.${MPIR_PATCHLEVEL_VERSION})
        endif()
    endforeach()

    # Check whether found version exists and exceeds the minimum requirement
    if(NOT MPIR_VERSION)
        set(MPIR_VERSION_OK FALSE)
        message(STATUS "MPIR version was not detected")
    elseif(${MPIR_VERSION} VERSION_LESS ${MPIR_FIND_VERSION})
        set(MPIR_VERSION_OK FALSE)
        message(STATUS "MPIR version ${MPIR_VERSION} found in ${MPIR_INCLUDE_DIRS}, "
                "but at least version ${MPIR_FIND_VERSION} is required")
    else()
        set(MPIR_VERSION_OK TRUE)
    endif()
endif()

find_library(MPIR_LIBRARIES gmp PATHS $ENV{MPIRDIR} ${LIB_INSTALL_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPIR DEFAULT_MSG
                                  MPIR_INCLUDE_DIRS MPIR_LIBRARIES MPIR_VERSION_OK)
if(MPIR_FOUND)
    set(HAVE_MPIR "${MPIR_FOUND}")
endif()

mark_as_advanced(MPIR_INCLUDE_DIRS MPIR_LIBRARIES)