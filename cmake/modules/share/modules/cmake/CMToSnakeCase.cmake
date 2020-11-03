#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

function(to_snake_case str var)
    # insert an underscore before any upper case letter
    # which is not followed by another upper case letter
    string(REGEX REPLACE "(.)([A-Z][a-z]+)" "\\1_\\2" value "${str}")
    # insert an underscore before any upper case letter
    # which is preseded by a lower case letter or number
    string(REGEX REPLACE "([a-z0-9])([A-Z])" "\\1_\\2" value "${value}")
    string(TOLOWER "${value}" value)
    set(${var} "${value}" PARENT_SCOPE)
endfunction()
