find_package( GMP QUIET )
find_package( MPFR QUIET )

if( GMP_FOUND AND MPFR_FOUND )

    if( MPFI_INCLUDES AND MPFI_LIBRARIES )
        set( MPFI_FOUND TRUE )
    endif( MPFI_INCLUDES AND MPFI_LIBRARIES )

    find_path(MPFI_INCLUDES NAMES mpfi.h
            HINTS ENV MPFI_INC_DIR
            ENV MPFI_DIR
            PATHS ${GMP_INCLUDE_DIR_SEARCH}
            PATH_SUFFIXES include
            DOC "The directory containing the MPFI header files"
            )
    if(MPFI_INCLUDES)
        file(READ "${MPFI_INCLUDES}/mpfi.h" _mpfr_version_header)

        string(REGEX MATCH "define[ \t]+MPFI_VERSION_MAJOR[ \t]+([0-9]+)"
                _mpfr_major_version_match "${_mpfr_version_header}")
        set(MPFI_MAJOR_VERSION "${CMAKE_MATCH_1}")
        string(REGEX MATCH "define[ \t]+MPFI_VERSION_MINOR[ \t]+([0-9]+)"
                _mpfr_minor_version_match "${_mpfr_version_header}")
        set(MPFI_MINOR_VERSION "${CMAKE_MATCH_1}")
        string(REGEX MATCH "define[ \t]+MPFI_VERSION_PATCHLEVEL[ \t]+([0-9]+)"
                _mpfr_patchlevel_version_match "${_mpfr_version_header}")
        set(MPFI_PATCHLEVEL_VERSION "${CMAKE_MATCH_1}")

        set(MPFI_VERSION
                ${MPFI_MAJOR_VERSION}.${MPFI_MINOR_VERSION}.${MPFI_PATCHLEVEL_VERSION})
    endif()


    find_library(MPFI_LIBRARIES NAMES mpfi
            HINTS ENV MPFI_LIB_DIR
            ENV MPFI_DIR
            PATHS ${GMP_LIBRARIES_DIR_SEARCH}
            PATH_SUFFIXES lib
            DOC "Directory containing the MPFI library"
            )

    if( MPFI_LIBRARIES )
        get_filename_component(MPFI_LIBRARIES_DIR ${MPFI_LIBRARIES} PATH CACHE )
    endif( MPFI_LIBRARIES )

    if( NOT MPFI_INCLUDES OR NOT MPFI_LIBRARIES_DIR )
        include( MPFIConfig OPTIONAL )
    endif( NOT MPFI_INCLUDES OR NOT MPFI_LIBRARIES_DIR )

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args( MPFI
            "DEFAULT_MSG"
            MPFI_LIBRARIES
            MPFI_INCLUDES )

else( GMP_FOUND AND MPFR_FOUND )

    message( STATUS "MPFI needs GMP and MPFR" )

endif( GMP_FOUND AND MPFR_FOUND )

if( MPFI_FOUND )
    if ("${MPFR_VERSION}" VERSION_LESS "4.0.0")
        set(_MPFR_OLD TRUE)
    endif()

    if ("${MPFI_VERSION}" VERSION_LESS "1.5.2")
        set(_MPFI_OLD TRUE)
    endif()

    if( ( _MPFR_OLD AND NOT _MPFI_OLD ) OR ( NOT _MPFR_OLD AND _MPFI_OLD ) )

        message(
                STATUS
                "MPFI<1.5.2 requires MPFR<4.0.0; MPFI>=1.5.2 requires MPFR>=4.0.0" )

        set( MPFI_FOUND FALSE )

    else( ( _MPFR_OLD AND NOT _MPFI_OLD ) OR ( NOT _MPFR_OLD AND _MPFI_OLD ) )

        set( MPFI_USE_FILE "CGAL_UseMPFI" )

    endif( ( _MPFR_OLD AND NOT _MPFI_OLD ) OR ( NOT _MPFR_OLD AND _MPFI_OLD ) )

endif( MPFI_FOUND )
