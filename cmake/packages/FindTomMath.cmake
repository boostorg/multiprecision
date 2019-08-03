#---------------------------------------------------------------------------#
# Copyright (c) 2018-2019 Nil Foundation AG
# Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
# Copyright (c) 2018-2019 Alexey Moskvin
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#
#
#  TomMath_FOUND - system has TomMath
#  TomMath_INCLUDE_DIRS - the TomMath include directories
#  TomMath_LIBRARY - link these to use TomMath

find_path(TomMath_INCLUDE_DIR tommath.h)

set(TomMath_NAMES ${TomMath_NAMES} tommath libtommath)
find_library(TomMath_LIBRARY NAMES ${TomMath_NAMES})

# handle the QUIETLY and REQUIRED arguments and set TIFF_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TomMath DEFAULT_MSG TomMath_LIBRARY TomMath_INCLUDE_DIR)

if(TOMMATH_FOUND)
    set(TomMath_FOUND true)
    set(TomMath_LIBRARIES ${TomMath_LIBRARY})
    set(TomMath_INCLUDES ${TomMath_INCLUDE_DIR})
endif(TOMMATH_FOUND)

mark_as_advanced(TomMath_INCLUDE_DIR TomMath_LIBRARY TomMath_FOUND)
