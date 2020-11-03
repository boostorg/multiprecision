#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

include(CMPkgConfig)
include(CMInstallTargets)
include(CMExport)

function(cm_deploy)
    set(options SKIP_HEADER_INSTALL)
    set(oneValueArgs NAMESPACE COMPATIBILITY)
    set(multiValueArgs TARGETS INCLUDE)

    cmake_parse_arguments(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(PARSE_SKIP_HEADER_INSTALL)
        cm_install_targets(TARGETS ${PARSE_TARGETS} INCLUDE ${PARSE_INCLUDE} SKIP_HEADER_INSTALL)
    else()
        cm_install_targets(TARGETS ${PARSE_TARGETS} INCLUDE ${PARSE_INCLUDE} )
    endif()

    cm_auto_pkgconfig(TARGET ${PARSE_TARGETS})
    cm_auto_export(TARGETS ${PARSE_TARGETS} NAMESPACE ${PARSE_NAMESPACE} COMPATIBILITY ${PARSE_COMPATIBILITY})

    foreach(TARGET ${PARSE_TARGETS})
        get_target_property(TARGET_NAME ${TARGET} EXPORT_NAME)
        if(NOT TARGET_NAME)
            get_target_property(TARGET_NAME ${TARGET} NAME)
        endif()
        set(EXPORT_LIB_TARGET ${PARSE_NAMESPACE}${TARGET_NAME})
        if(NOT TARGET ${EXPORT_LIB_TARGET})
            add_library(${EXPORT_LIB_TARGET} ALIAS ${TARGET})
        endif()
        set_target_properties(${TARGET} PROPERTIES INTERFACE_FIND_PACKAGE_NAME ${PROJECT_NAME})
        if(COMMAND cm_add_rpath)
            get_target_property(TARGET_TYPE ${TARGET} TYPE)
            if(NOT "${TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
                cm_add_rpath("$<TARGET_FILE_DIR:${TARGET}>")
            endif()
        endif()
        cm_shadow_notify(${EXPORT_LIB_TARGET})
        cm_shadow_notify(${TARGET})
    endforeach()

endfunction()
