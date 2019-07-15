# Try to find the MPIRXX libraries
# MPIRXX_FOUND - system has MPIRXX lib
# MPIRXX_INCLUDE_DIR - the MPIRXX include directory
# MPIRXX_LIBRARIES - Libraries needed to use MPIRXX

# TODO: support Windows and MacOSX

# MPIRXX needs MPIR

find_package(MPIR QUIET)

if(MPIR_FOUND)
    if(MPIRXX_INCLUDE_DIR AND MPIRXX_LIBRARIES)
        # Already in cache, be silent
        set(MPIRXX_FIND_QUIETLY TRUE)
    endif()

    find_path(MPIRXX_INCLUDE_DIR NAMES mpirxx.h
              PATHS ${MPIR_INCLUDE_DIR_SEARCH}
              DOC "The directory containing the MPIRXX include files"
              )

    find_library(MPIRXX_LIBRARIES NAMES mpirxx
                 PATHS ${MPIR_LIBRARIES_DIR_SEARCH}
                 DOC "Path to the MPIRXX library"
                 )


    find_package_handle_standard_args(MPIRXX "DEFAULT_MSG" MPIRXX_LIBRARIES MPIRXX_INCLUDE_DIR)

endif()
