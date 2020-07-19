#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2018-2020 Alexey Moskvin
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#
#
# Try to find the GMPXX libraries
#
# Once done this will define
# GMPXX_FOUND - system has GMPXX lib
# GMPXX_INCLUDE_DIRS - the GMPXX include directory
# GMPXX_LIBRARIES - Libraries needed to use GMPXX

if(GMP_FOUND)
    find_package(GMP QUIET)
else()
    find_package(GMP)
endif()

if(GMP_FOUND)
    if(GMPXX_INCLUDE_DIRS AND GMPXX_LIBRARIES)
        # Already in cache, be silent
        set(GMPXX_FIND_QUIETLY TRUE)
    endif()

    find_path(GMPXX_INCLUDE_DIRS NAMES gmpxx.h
              PATHS ${GMP_INCLUDE_DIRS_SEARCH}
              DOC "The directory containing the GMPXX include files")

    find_library(GMPXX_LIBRARIES NAMES gmpxx
                 PATHS ${GMP_LIBRARIES_DIR_SEARCH}
                 DOC "Path to the GMPXX library")

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(GMPXX "DEFAULT_MSG" GMPXX_LIBRARIES GMPXX_INCLUDE_DIRS)
endif()
