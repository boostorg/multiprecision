#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

# Try to find Eigen3 library
# http://eigen.tuxfamily.org
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(Eigen3 3.1.2)
# to require version 3.1.2 or newer of Eigen3.
#
# Once done this will define
#
#  EIGEN3_FOUND - system has eigen lib with correct version
#  EIGEN3_INCLUDE_DIR - the eigen include directory
#  EIGEN3_VERSION - eigen version

if(NOT Eigen3_FIND_VERSION)
    if(NOT Eigen3_FIND_VERSION_MAJOR)
        set(Eigen3_FIND_VERSION_MAJOR 2)
    endif(NOT Eigen3_FIND_VERSION_MAJOR)
    if(NOT Eigen3_FIND_VERSION_MINOR)
        set(Eigen3_FIND_VERSION_MINOR 91)
    endif(NOT Eigen3_FIND_VERSION_MINOR)
    if(NOT Eigen3_FIND_VERSION_PATCH)
        set(Eigen3_FIND_VERSION_PATCH 0)
    endif(NOT Eigen3_FIND_VERSION_PATCH)

    set(Eigen3_FIND_VERSION "${Eigen3_FIND_VERSION_MAJOR}.${Eigen3_FIND_VERSION_MINOR}.${Eigen3_FIND_VERSION_PATCH}")
endif(NOT Eigen3_FIND_VERSION)

macro(_eigen3_check_version)
    file(READ "${EIGEN3_INCLUDE_DIRS}/Eigen/src/Core/util/Macros.h" _eigen3_version_header)

    string(REGEX MATCH "define[ \t]+EIGEN_WORLD_VERSION[ \t]+([0-9]+)" _eigen3_world_version_match "${_eigen3_version_header}")
    set(EIGEN3_WORLD_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+EIGEN_MAJOR_VERSION[ \t]+([0-9]+)" _eigen3_major_version_match "${_eigen3_version_header}")
    set(EIGEN3_MAJOR_VERSION "${CMAKE_MATCH_1}")
    string(REGEX MATCH "define[ \t]+EIGEN_MINOR_VERSION[ \t]+([0-9]+)" _eigen3_minor_version_match "${_eigen3_version_header}")
    set(EIGEN3_MINOR_VERSION "${CMAKE_MATCH_1}")

    set(EIGEN3_VERSION ${EIGEN3_WORLD_VERSION}.${EIGEN3_MAJOR_VERSION}.${EIGEN3_MINOR_VERSION})
    if(${EIGEN3_VERSION} VERSION_LESS ${Eigen3_FIND_VERSION})
        set(EIGEN3_VERSION_OK FALSE)
    else(${EIGEN3_VERSION} VERSION_LESS ${Eigen3_FIND_VERSION})
        set(EIGEN3_VERSION_OK TRUE)
    endif(${EIGEN3_VERSION} VERSION_LESS ${Eigen3_FIND_VERSION})

    if(NOT EIGEN3_VERSION_OK)

        message(STATUS "Eigen3 version ${EIGEN3_VERSION} found in ${EIGEN3_INCLUDE_DIRS}, "
                "but at least version ${Eigen3_FIND_VERSION} is required")
    endif(NOT EIGEN3_VERSION_OK)
endmacro(_eigen3_check_version)

if(EIGEN3_INCLUDE_DIRS)

    # in cache already
    _eigen3_check_version()
    set(EIGEN3_FOUND ${EIGEN3_VERSION_OK})

else(EIGEN3_INCLUDE_DIRS)

    # specific additional paths for some OS
    if(WIN32)
        set(EIGEN_ADDITIONAL_SEARCH_PATHS ${EIGEN_ADDITIONAL_SEARCH_PATHS} "C:/Program Files/Eigen/include" "C:/Program Files (x86)/Eigen/include")
    endif(WIN32)

    find_path(EIGEN3_INCLUDE_DIRS NAMES signature_of_eigen3_matrix_library
              PATHS
              include
              ${EIGEN_ADDITIONAL_SEARCH_PATHS}
              ${KDE4_INCLUDE_DIR}
              PATH_SUFFIXES eigen3 eigen
              )

    if(EIGEN3_INCLUDE_DIRS)
        _eigen3_check_version()
    endif(EIGEN3_INCLUDE_DIRS)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Eigen3 DEFAULT_MSG EIGEN3_INCLUDE_DIRS EIGEN3_VERSION_OK)

    mark_as_advanced(EIGEN3_INCLUDE_DIRS)

endif(EIGEN3_INCLUDE_DIRS)