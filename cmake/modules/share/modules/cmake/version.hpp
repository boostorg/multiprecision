//---------------------------------------------------------------------------//
//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef @PREFIX@_GUARD_VERSION_HPP
#define @PREFIX@_GUARD_VERSION_HPP

#define @PREFIX@_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define @PREFIX@_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define @PREFIX@_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define @PREFIX@_VERSION (((@PREFIX@_VERSION_MAJOR) << 24) + ((@PREFIX@_VERSION_MINOR) << 16) + (@PREFIX@_VERSION_PATCH))

#endif
