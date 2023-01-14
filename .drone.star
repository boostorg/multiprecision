# Use, modification, and distribution are
# subject to the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt)
#
# Copyright Rene Rivera 2020.

# For Drone CI we use the Starlark scripting language to reduce duplication.
# As the yaml syntax for Drone CI is rather limited.
#
#
globalenv={}
linuxglobalimage="cppalliance/droneubuntu1604:1"
windowsglobalimage="cppalliance/dronevs2019"

def main(ctx):
  return [
  
  linux_cxx("Ubuntu g++ std=gnu++17 arithmetic_tests", "g++", packages="libgmp-dev libmpfr-dev libmpc-dev libmpfi-dev libtommath-dev", buildtype="boost", image="cppalliance/droneubuntu2204:multiarch", arch="arm64", environment={'TOOLSET': 'gcc', 'COMPILER': 'g++', 'CXXSTD': 'gnu++17', 'TEST_SUITE': 'arithmetic_tests', }, globalenv=globalenv),
  linux_cxx("Ubuntu g++ std=gnu++17 cpp_int_tests", "g++", packages="libgmp-dev libmpfr-dev libmpc-dev libmpfi-dev libtommath-dev", buildtype="boost", image="cppalliance/droneubuntu2204:multiarch", arch="arm64", environment={'TOOLSET': 'gcc', 'COMPILER': 'g++', 'CXXSTD': 'gnu++17', 'TEST_SUITE': 'cpp_int_tests', }, globalenv=globalenv),
  linux_cxx("Ubuntu g++ std=gnu++17 functions_and_limits", "g++", packages="libgmp-dev libmpfr-dev libmpc-dev libmpfi-dev libtommath-dev", buildtype="boost", image="cppalliance/droneubuntu2204:multiarch", arch="arm64", environment={'TOOLSET': 'gcc', 'COMPILER': 'g++', 'CXXSTD': 'gnu++17', 'TEST_SUITE': 'functions_and_limits', }, globalenv=globalenv),
  linux_cxx("Ubuntu g++ std=gnu++17 conversions performance", "g++", packages="libgmp-dev libmpfr-dev libmpc-dev libmpfi-dev libtommath-dev", buildtype="boost", image="cppalliance/droneubuntu2204:multiarch", arch="arm64", environment={'TEST_SUITE': 'conversions performance', 'TOOLSET': 'gcc', 'COMPILER': 'g++', 'CXXSTD': 'gnu++17', }, globalenv=globalenv),
  linux_cxx("Ubuntu g++ std=gnu++17 misc", "g++", packages="libgmp-dev libmpfr-dev libmpc-dev libmpfi-dev libtommath-dev", buildtype="boost", image="cppalliance/droneubuntu2204:multiarch", arch="arm64", environment={'TOOLSET': 'gcc', 'COMPILER': 'g++', 'CXXSTD': 'gnu++17', 'TEST_SUITE': 'misc', }, globalenv=globalenv),
  linux_cxx("Ubuntu g++ std=gnu++17 compile_fail examples", "g++", packages="libgmp-dev libmpfr-dev libmpc-dev libmpfi-dev libtommath-dev", buildtype="boost", image="cppalliance/droneubuntu2204:multiarch", arch="arm64", environment={'TEST_SUITE': 'compile_fail examples', 'TOOLSET': 'gcc', 'COMPILER': 'g++', 'CXXSTD': 'gnu++17', }, globalenv=globalenv),
  linux_cxx("Ubuntu g++ std=gnu++17 concepts", "g++", packages="libgmp-dev libmpfr-dev libmpc-dev libmpfi-dev libtommath-dev", buildtype="boost", image="cppalliance/droneubuntu2204:multiarch", arch="arm64", environment={'TOOLSET': 'gcc', 'COMPILER': 'g++', 'CXXSTD': 'gnu++17', 'TEST_SUITE': 'concepts', }, globalenv=globalenv),

  ]

# from https://github.com/boostorg/boost-ci
load("@boost_ci//ci/drone/:functions.star", "linux_cxx","windows_cxx","osx_cxx","freebsd_cxx")
