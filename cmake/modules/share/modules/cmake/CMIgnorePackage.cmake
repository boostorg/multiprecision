
function(cm_ignore_package NAME)
    set(${NAME}_DIR ${CMAKE_BINARY_DIR}/_cm_ignore_packages_/${NAME} CACHE PATH "")
    file(WRITE ${${NAME}_DIR}/${NAME}Config.cmake "")
endfunction()

