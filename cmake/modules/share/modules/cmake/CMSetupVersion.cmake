include(GNUInstallDirs)
set(CM_HEADER_VERSION_TEMPLATE_FILE "${CMAKE_CURRENT_LIST_DIR}/version.hpp")

macro(cm_set_parent VAR)
    set(${VAR} ${ARGN} PARENT_SCOPE)
    set(${VAR} ${ARGN})
endmacro()

function(cm_setup_version)
    set(options)
    set(oneValueArgs VERSION GENERATE_HEADER PARSE_HEADER PREFIX)
    set(multiValueArgs)

    cmake_parse_arguments(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    string(TOUPPER ${PROJECT_NAME} PREFIX)

    if(PARSE_PREFIX)
        set(PREFIX ${PARSE_PREFIX})
    endif()

    if(PARSE_VERSION)
        cm_set_parent(PROJECT_VERSION ${PARSE_VERSION})
        cm_set_parent(${PROJECT_NAME}_VERSION ${PROJECT_VERSION})
        string(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" _version_MAJOR "${PROJECT_VERSION}")
        string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" _version_MINOR "${PROJECT_VERSION}")
        string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" _version_PATCH "${PROJECT_VERSION}")
        foreach(level MAJOR MINOR PATCH)
            cm_set_parent(${PROJECT_NAME}_VERSION_${level} ${_version_${level}})
            cm_set_parent(PROJECT_VERSION_${level} ${_version_${level}})
        endforeach()
    elseif(PARSE_PARSE_HEADER)
        foreach(level MAJOR MINOR PATCH)
            file(STRINGS ${PARSE_PARSE_HEADER}
                         _define_${level}
                         REGEX "#define ${PREFIX}_VERSION_${level}")
            string(REGEX MATCH "([0-9]+)" _version_${level} "${_define_${level}}")
            # TODO: Check if it is empty
            cm_set_parent(${PROJECT_NAME}_VERSION_${level} ${_version_${level}})
            cm_set_parent(PROJECT_VERSION_${level} ${_version_${level}})
        endforeach()
        cm_set_parent(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
        cm_set_parent(${PROJECT_NAME}_VERSION ${PROJECT_VERSION})
    endif()
    # TODO: Get version from the project

    if(PARSE_GENERATE_HEADER)
        configure_file("${CM_HEADER_VERSION_TEMPLATE_FILE}" "${PARSE_GENERATE_HEADER}")
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PARSE_GENERATE_HEADER}" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    endif()

endfunction()
