# - Try to find TomMath
# Once done, this will define
#
#  TomMath_FOUND - system has TomMath
#  TomMath_INCLUDE_DIRS - the TomMath include directories
#  TomMath_LIBRARY - link these to use TomMath

FIND_PATH(TomMath_INCLUDE_DIR tommath.h)

SET(TomMath_NAMES ${TomMath_NAMES} tommath libtommath )
FIND_LIBRARY(TomMath_LIBRARY NAMES ${TomMath_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set TIFF_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)



FIND_PACKAGE_HANDLE_STANDARD_ARGS(TomMath  DEFAULT_MSG  TomMath_LIBRARY  TomMath_INCLUDE_DIR)

IF(TOMMATH_FOUND)
  set( TomMath_FOUND true )
  SET( TomMath_LIBRARIES ${TomMath_LIBRARY} )
ENDIF(TOMMATH_FOUND)

MARK_AS_ADVANCED(TomMath_INCLUDE_DIR TomMath_LIBRARY TomMath_FOUND)
