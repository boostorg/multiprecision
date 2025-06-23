# Copyright 2018, 2019 Peter Dimov
# Copyright 2024 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

file(GLOB SOURCES "*.cpp")
add_library(boost_mp_tests STATIC ${SOURCES})
target_compile_features(boost_mp_tests PRIVATE cxx_std_17)
target_link_libraries(boost_mp_tests PUBLIC Boost::multiprecision Boost::assert Boost::config Boost::core Boost::integer Boost::lexical_cast Boost::math Boost::random)
