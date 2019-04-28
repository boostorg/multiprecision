# Copyright 2019 Sam Day
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
#
# NOTE: CMake support for Boost.multiprecision is currently experimental at best
#       and the interface is likely to change in the future

cmake_minimum_required(VERSION 3.5)
project(BoostMultiprecision LANGUAGES CXX)

add_library(boost_multiprecision INTERFACE)
add_library(Boost::multiprecision ALIAS boost_multiprecision)

target_include_directories(boost_multiprecision INTERFACE include)

target_link_libraries(boost_multiprecision
        INTERFACE
        Boost::array
        Boost::assert
        Boost::config
        Boost::container_hash
        Boost::core
        Boost::functional
        Boost::integer
        Boost::lexical_cast
        Boost::math
        Boost::mpl
        Boost::predef
        Boost::random
        Boost::rational
        Boost::smart_ptr
        Boost::static_assert
        Boost::throw_exception
        Boost::type_traits
        Boost::utility
        )
