#---------------------------------------------------------------------------#
# Copyright (c) 2017 Niall Douglas
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#
#
# On MSVC very annoyingly cmake puts /EHsc and /MD(d) into the global flags which means you
# get a warning when you try to disable exceptions or use the static CRT. I hate to use this
# globally imposed solution, but we are going to hack the global flags to use properties to
# determine whether they are on or off
# 
# Create custom properties called CXX_EXCEPTIONS, CXX_RTTI and CXX_STATIC_RUNTIME
# These get placed at global, directory and target scopes
foreach(scope GLOBAL DIRECTORY TARGET)
    define_property(${scope} PROPERTY "CXX_EXCEPTIONS" INHERITED
                    BRIEF_DOCS "Enable C++ exceptions, defaults to TRUE at global scope"
                    FULL_DOCS "Enable C++ exceptions, defaults to TRUE at global scope"
                    )
    define_property(${scope} PROPERTY "CXX_RTTI" INHERITED
                    BRIEF_DOCS "Enable C++ runtime type information, defaults to TRUE at global scope"
                    FULL_DOCS "Enable C++ runtime type information, defaults to TRUE at global scope"
                    )
    define_property(${scope} PROPERTY "CXX_STATIC_RUNTIME" INHERITED
                    BRIEF_DOCS "Enable linking against the static C++ runtime, defaults to FALSE at global scope"
                    FULL_DOCS "Enable linking against the static C++ runtime, defaults to FALSE at global scope"
                    )
    define_property(${scope} PROPERTY "CXX_WARNINGS" INHERITED
                    BRIEF_DOCS "Controls the warning level of compilers, defaults to TRUE at global scope"
                    FULL_DOCS "Controls the warning level of compilers, defaults to TRUE at global scope"
                    )
    define_property(${scope} PROPERTY "CXX_WARNINGS_AS_ERRORS" INHERITED
                    BRIEF_DOCS "Treat warnings as errors and abort compilation on a warning, defaults to FALSE at global scope"
                    FULL_DOCS "Treat warnings as errors and abort compilation on a warning, defaults to FALSE at global scope"
                    )
    define_property(${scope} PROPERTY "BUILD_SHARED_LIBS" INHERITED
                    BRIEF_DOCS "Property indicates wether dynamic-linked libraries are going to be built, defaults to
                     FALSE at global scope"
                    FULL_DOCS "Property indicates wether dynamic-linked libraries are going to be built, defaults to
                     FALSE at global scope"
                    )
endforeach()
# Set the default for these properties at global scope. If they are not set per target or
# whatever, the next highest scope will be looked up
option(CMAKE_CXX_EXCEPTIONS "Enable C++ exceptions, defaults to TRUE at global scope" TRUE)
option(CMAKE_CXX_RTTI "Enable C++ runtime type information, defaults to TRUE at global scope" TRUE)
option(CMAKE_CXX_STATIC_RUNTIME "Enable linking against the static C++ runtime, defaults to FALSE at global scope" FALSE)
option(CMAKE_CXX_WARNINGS "Controls the warning level of compilers, defaults to TRUE at global scope" TRUE)
option(CMAKE_CXX_WARNINGS_AS_ERRORS "Treat warnings as errors and abort compilation on a warning, defaults to FALSE at global scope" FALSE)

set_property(GLOBAL PROPERTY CXX_EXCEPTIONS ${CMAKE_CXX_EXCEPTIONS})
set_property(GLOBAL PROPERTY CXX_RTTI ${CMAKE_CXX_RTTI})
set_property(GLOBAL PROPERTY CXX_STATIC_RUNTIME ${CMAKE_CXX_STATIC_RUNTIME})
set_property(GLOBAL PROPERTY CXX_WARNINGS ${CMAKE_CXX_WARNINGS})
set_property(GLOBAL PROPERTY CXX_WARNINGS_AS_ERRORS ${CMAKE_CXX_WARNINGS_AS_ERRORS})
set_property(GLOBAL PROPERTY BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

if(MSVC)
    # Purge unconditional use of /MDd, /MD and /EHsc.
    foreach(flag
            CMAKE_C_FLAGS CMAKE_CXX_FLAGS
            CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_RELWITHDEBINFO
            )
        string(REPLACE "/MDd" "" ${flag} "${${flag}}")
        string(REPLACE "/MD" "" ${flag} "${${flag}}")
        string(REPLACE "/EHsc" "" ${flag} "${${flag}}")
        string(REPLACE "/GR" "" ${flag} "${${flag}}")
    endforeach()
    # Restore those same, but now selected by the properties
    add_compile_options(
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_EXCEPTIONS>>,TRUE>:/EHsc>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_RTTI>>,FALSE>:/GR->
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_STATIC_RUNTIME>>,FALSE>:$<$<CONFIG:Debug>:/MDd>$<$<NOT:$<CONFIG:Debug>>:/MD>>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_STATIC_RUNTIME>>,TRUE>:$<$<CONFIG:Debug>:/MTd>$<$<NOT:$<CONFIG:Debug>>:/MT>>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,TRUE>:/W3>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,FALSE>:/W0>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:/W4>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS_AS_ERRORS>>,TRUE>:/WX>
    )
else()
    add_compile_options(
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_EXCEPTIONS>>,FALSE>:-fno-exceptions>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_RTTI>>,FALSE>:-fno-rtti>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_STATIC_RUNTIME>>,TRUE>:-static>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,TRUE>:-Wall>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,FALSE>:-w>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Wall>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-pedantic>
            $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS_AS_ERRORS>>,TRUE>:-Werror>
    )
    if(CMAKE_${COMPILER}_COMPILER_ID MATCHES "Clang" OR CMAKE_${COMPILER}_COMPILER_ID MATCHES "AppleClang")
        add_compile_options(
                $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Weverything>
                $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Wno-macro-redefined>
                $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Wall>
                $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Wno-c++98-compat>
                $<$<STREQUAL:$<UPPER_CASE:$<TARGET_PROPERTY:CXX_WARNINGS>>,ALL>:-Wno-c++98-compat-pedantic>
        )
    endif()
endif()