
==========
Motivation
==========

This provides cmake modules that can be re-used by boost and other dependencies. It provides modules to reduce the boilerplate for installing, versioning, setting up package config, and creating tests.

=====
Usage
=====

The modules can be installed using standard cmake install::

    mkdir build
    cd build
    cmake ..
    cmake --build . --target install

Once installed, the modules can be used by using ``find_package`` and then including the appropriate module::

    find_package(CM)
    include(CMDeploy)

===========
Quick Start
===========

------------------------
Building a boost library
------------------------

The BCM modules provide some high-level cmake functions to take care of all the cmake boilerplate needed to build, install and configuration setup. To setup a simple boost library we can do::

    cmake_minimum_required (VERSION 3.5)
    project(boost_config)
    
    find_package(CM)
    include(CMDeploy)
    include(CMSetupVersion)

    cm_setup_version(VERSION 1.58.0)

    add_library(boost_config INTERFACE)
    add_library(boost::config ALIAS boost_config)
    set_property(TARGET boost_config PROPERTY EXPORT_NAME config)

    cm_deploy(TARGETS config INCLUDE include)
    

This sets up the Boost.Config cmake with the version ``1.64.0``. More importantly the user can now install the library, like this::

    mkdir build
    cd build
    cmake ..
    cmake --build . --target install

And then the user can build with Boost.Config using cmake's ``find_package``::

    project(foo)

    find_package(boost_config)
    add_executable(foo foo.cpp)
    target_link_libraries(foo boost::config)

Or if the user isn't using cmake, then ``pkg-config`` can be used instead::

    g++ `pkg-config boost_config --cflags --libs` foo.cpp

-----
Tests
-----

The BCM modules provide functions for creating tests that integrate into cmake's ctest infrastructure. All tests can be built and ran using ``make check``. The ``cm_test`` function can add a test to be ran::

    cm_test(NAME config_test_c SOURCES config_test_c.c)

This will compile the ``SOURCES`` and run them. The test also needs to link in ``boost_config``. This can be done with ``target_link_libraries``::

    target_link_libraries(config_test_c boost::config)

Or all tests in the directory can be set using ``cm_test_link_libraries``::

    cm_test_link_libraries(boost::config)

And all tests in the directory will use ``boost::config``.

Also, tests can be specified as compile-only or as expected to fail::

    cm_test(NAME test_thread_fail1 SOURCES threads/test_thread_fail1.cpp COMPILE_ONLY WILL_FAIL)
