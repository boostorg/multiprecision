#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2018-2020 Alexey Moskvin
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#
#
# Try to find the MPIRXX libraries
# MPIRXX_FOUND - system has MPIRXX lib
# MPIRXX_INCLUDE_DIRS - the MPIRXX include directory
# MPIRXX_LIBRARIES - Libraries needed to use MPIRXX

# MPIRXX needs MPIR
find_package(MPIR QUIET)

if(MPIR_FOUND)
    if(MPIRXX_INCLUDE_DIRS AND MPIRXX_LIBRARIES)
        # Already in cache, be silent
        set(MPIRXX_FIND_QUIETLY TRUE)
    endif()

    find_path(MPIRXX_INCLUDE_DIRS NAMES mpirxx.h
              PATHS ${MPIR_INCLUDE_DIR_SEARCH}
              DOC "The directory containing the MPIRXX include files"
              )

    find_library(MPIRXX_LIBRARIES NAMES mpirxx
                 PATHS ${MPIR_LIBRARIES_DIR_SEARCH}
                 DOC "Path to the MPIRXX library"
                 )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(MPIRXX "DEFAULT_MSG" MPIRXX_LIBRARIES MPIRXX_INCLUDE_DIRS)
endif()
