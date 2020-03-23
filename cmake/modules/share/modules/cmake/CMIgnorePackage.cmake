#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

function(cm_ignore_package NAME)
    set(${NAME}_DIR ${CMAKE_BINARY_DIR}/_cm_ignore_packages_/${NAME} CACHE PATH "")
    file(WRITE ${${NAME}_DIR}/${NAME}Config.cmake "")
endfunction()

