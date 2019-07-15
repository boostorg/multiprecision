========
Building
========

There are two scenarios where the users will consume their dependencies in the build:

* Prebuilt binaries using ``find_package``
* Integrated builds using ``add_subdirectory``

When we build libraries using cmake, we want to be able to support both scenarios. 

The first scenario the user would build and install each dependency. With this scenario, we need to generate usage requirements that can be consumed by the user, and ultimately this is done through cmake's ``find_package`` mechanism.

In the integrated build scenario, the user adds the sources with ``add_subdirectory``, and then all dependencies are built in the user's build. There is no need to generate usage requirements as the cmake targets are directly available in the build.

Let's first look at standalone build. 

------------------------------
Building standalone with cmake
------------------------------

Let's look at building a library like Boost.Filesystem using just cmake. When we start a cmake, we start with minimuim requirement and the project name::

    cmake_minimum_required(VERSION 3.5)
    project(boost_filesystem)

Then we can define the library and the sources it will build::

    add_library(boost_filesystem 
        src/operations.cpp
        src/portability.cpp
        src/codecvt_error_category.cpp
        src/utf8_codecvt_facet.cpp
        src/windows_file_codecvt.cpp
        src/unique_path.cpp
        src/path.cpp
        src/path_traits.cpp
    )

So this will build the library named ``boost_filesystem``, however, we need to supply the dependencies to ``boost_filesystem`` and add the include directories. To add the include directory we use ``target_include_directories``. For this, we tell cmake to use local ``include`` directory, but since this is only valid during build and not after installation, we use the ``BUILD_INTERFACE`` generator expression so that cmake will only use it during build and not installation::

    target_include_directories(boost_filesystem PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

Using ``PUBLIC`` means this include directory will be used internally to build, and downstream users need this include as well. Next, we need to pull in the dependencies. To do this, we call ``find_package``, and for the sake of the turtorial we assume that the upstream boost libraries have already set this up::

    find_package(boost_core)
    find_package(boost_static_assert)
    find_package(boost_iterator)
    find_package(boost_detail)
    find_package(boost_system)
    find_package(boost_functional)
    find_package(boost_assert)
    find_package(boost_range)
    find_package(boost_type_traits)
    find_package(boost_smart_ptr)
    find_package(boost_io)
    find_package(boost_config)

Calling ``find_package`` will find those libraries and provide a target we can use to link against. The next step is to link it using ``target_link_libraries``::

    target_link_libraries(boost_filesystem PUBLIC 
        boost::core
        boost::static_assert
        boost::iterator
        boost::detail
        boost::system
        boost::functional
        boost::assert
        boost::range
        boost::type_traits
        boost::smart_ptr
        boost::io
        boost::config
    )

Now, some of these libraries are header-only, but when we call ``target_link_libraries`` it will add all the flags necessary to use those libraries. Next step is installation, using the ``install`` command::

    install(DIRECTORY include/ DESTINATION include)

    install(TARGETS boost_filesystem EXPORT boost_filesystem-targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
    )

So this will install the include directories and install the library. The ``EXPORT`` command will have cmake generate an export file that will create the target's usage requirements in cmake. This will enable the target to be used by downstream libraries, just like we used ```boost::system``. However, this will only tells cmake which targets are in the export file. To generate it we use ``install(EXPORT)``::

    install(EXPORT boost_filesystem-targets
        FILE boost_filesystem-targets.cmake
        NAMESPACE boost::
        DESTINATION lib/cmake/boost_filesystem
    )

This sets a namespace ``boost::`` on the target, but our target is named ``boost_filesystem``, and we want the exported target to be ``boost::filesystem`` not ``boost::boost_filesystem``. We can do that by setting the export name::

    set_property(TARGET boost_filesystem PROPERTY EXPORT_NAME filesystem)

We can also define a target alias to ``boost::filesystem``, which helps integrated builds::

    add_library(boost::filesystem ALIAS boost_filesystem)

So now have exported targets we want to generate a ``boost_filesystem-config.cmake`` file so it can be used with ``find_package(boost_filesystem)``. To do this we generate a file the includes the export file, but it also calls ``find_dependency`` on each dependency so that the user does not have to call it::

    file(WRITE "${PROJECT_BINARY_DIR}/boost_filesystem-config.cmake" "
    include(CMakeFindDependencyMacro)
    find_dependency(boost_core)
    find_dependency(boost_static_assert)
    find_dependency(boost_iterator)
    find_dependency(boost_detail)
    find_dependency(boost_system)
    find_dependency(boost_functional)
    find_dependency(boost_assert)
    find_dependency(boost_range)
    find_dependency(boost_type_traits)
    find_dependency(boost_smart_ptr)
    find_dependency(boost_io)
    find_dependency(boost_config)
    include(\"\${CMAKE_CURRENT_LIST_DIR}/boost_filesystem-targets.cmake\")
    ")

Besides the ``boost_filesystem-config.cmake``, we also need a version file to check compatibility. This can be done using cmake's ``write_basic_package_version_file`` function::

    write_basic_package_version_file("${PROJECT_BINARY_DIR}/boost_filesystem-config-version.cmake"
        VERSION 1.64
        COMPATIBILITY AnyNewerVersion
    )

Then finally we install these files::

    install(FILES
        "${PROJECT_BINARY_DIR}/boost_filesystem-config.cmake"
        "${PROJECT_BINARY_DIR}/boost_filesystem-config-version.cmake"
        DESTINATION lib/cmake/boost_filesystem
    )

Putting it all together we have a cmake file that looks like this::

    cmake_minimum_required(VERSION 3.5)
    project(boost_filesystem)
    include(CMakePackageConfigHelpers)

    find_package(boost_core)
    find_package(boost_static_assert)
    find_package(boost_iterator)
    find_package(boost_detail)
    find_package(boost_system)
    find_package(boost_functional)
    find_package(boost_assert)
    find_package(boost_range)
    find_package(boost_type_traits)
    find_package(boost_smart_ptr)
    find_package(boost_io)
    find_package(boost_config)

    add_library(boost_filesystem 
      src/operations.cpp
      src/portability.cpp
      src/codecvt_error_category.cpp
      src/utf8_codecvt_facet.cpp
      src/windows_file_codecvt.cpp
      src/unique_path.cpp
      src/path.cpp
      src/path_traits.cpp
    )
    add_library(boost::filesystem ALIAS boost_filesystem)
    set_property(TARGET boost_filesystem PROPERTY EXPORT_NAME filesystem)

    target_include_directories(boost_filesystem PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
    target_link_libraries(boost_filesystem PUBLIC 
        boost::core
        boost::static_assert
        boost::iterator
        boost::detail
        boost::system
        boost::functional
        boost::assert
        boost::range
        boost::type_traits
        boost::smart_ptr
        boost::io
        boost::config
    )


    install(DIRECTORY include/ DESTINATION include)

    install(TARGETS boost_filesystem EXPORT boost_filesystem-targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
    )

    install(EXPORT boost_filesystem-targets
        FILE boost_filesystem-targets.cmake
        NAMESPACE boost::
        DESTINATION lib/cmake/boost_filesystem
    )

    file(WRITE "${PROJECT_BINARY_DIR}/boost_filesystem-config.cmake" "
    include(CMakeFindDependencyMacro)
    find_dependency(boost_core)
    find_dependency(boost_static_assert)
    find_dependency(boost_iterator)
    find_dependency(boost_detail)
    find_dependency(boost_system)
    find_dependency(boost_functional)
    find_dependency(boost_assert)
    find_dependency(boost_range)
    find_dependency(boost_type_traits)
    find_dependency(boost_smart_ptr)
    find_dependency(boost_io)
    find_dependency(boost_config)
    include(\"\${CMAKE_CURRENT_LIST_DIR}/boost_filesystem-targets.cmake\")
    ")

    write_basic_package_version_file("${PROJECT_BINARY_DIR}/boost_filesystem-config-version.cmake"
        VERSION 1.64
        COMPATIBILITY AnyNewerVersion
    )

    install(FILES
        "${PROJECT_BINARY_DIR}/boost_filesystem-config.cmake"
        "${PROJECT_BINARY_DIR}/boost_filesystem-config-version.cmake"
        DESTINATION lib/cmake/boost_filesystem
    )

---------------------------
Building standalone with CM
---------------------------

The boost cmake modules can help reduce the boilerplate needed in writing these libraries. To use these modules we just call ``find_package(CM)`` first::

    cmake_minimum_required(VERSION 3.5)
    project(boost_filesystem)
    find_package(CM)

Next we can setup the version for the project using ``cm_setup_version``::

    cm_setup_version(VERSION 1.64)

Next, we add the library and link against the dependencies like always::

    find_package(boost_core)
    find_package(boost_static_assert)
    find_package(boost_iterator)
    find_package(boost_detail)
    find_package(boost_system)
    find_package(boost_functional)
    find_package(boost_assert)
    find_package(boost_range)
    find_package(boost_type_traits)
    find_package(boost_smart_ptr)
    find_package(boost_io)
    find_package(boost_config)

    add_library(boost_filesystem 
      src/operations.cpp
      src/portability.cpp
      src/codecvt_error_category.cpp
      src/utf8_codecvt_facet.cpp
      src/windows_file_codecvt.cpp
      src/unique_path.cpp
      src/path.cpp
      src/path_traits.cpp
    )
    add_library(boost::filesystem ALIAS boost_filesystem)
    set_property(TARGET boost_filesystem PROPERTY EXPORT_NAME filesystem)

    target_link_libraries(boost_filesystem PUBLIC 
        boost::core
        boost::static_assert
        boost::iterator
        boost::detail
        boost::system
        boost::functional
        boost::assert
        boost::range
        boost::type_traits
        boost::smart_ptr
        boost::io
        boost::config
    )

Then to install, and generate package configuration we just use ``cm_deploy``::

    cm_deploy(TARGETS boost_filesystem NAMESPACE boost::)

In addition to generating package configuration for cmake, this will also generate the package configuration for ``pkgconfig``.

-----------------
Integrated builds
-----------------

As we were setting up cmake for standalone builds, we made sure we didn't do anything to prevent an integrated build, and even provided an alias target to help ease the process. Finally, to integrate the sources into the build is just a matter of calling ``add_subdirectory`` on each project::

    file(GLOB LIBS libs/*)
    foreach(lib ${LIBS})
        add_subdirectory(${lib})
    endforeach()

We could also use ``add_subdirectory(${lib} EXCLUDE_FROM_ALL)`` so it builds targets that are not necessary. Of course, every project is still calling ``find_package`` to find prebuilt binaries. Since we don't need to search for those libraries because they are integrated into the build we can call ``cm_ignore_package`` to ignore those dependencies::

    file(GLOB LIBS libs/*)

    foreach(lib ${LIBS})
        cm_ignore_package(${lib})
    endforeach()

    foreach(lib ${LIBS})
        add_subdirectory(${lib})
    endforeach()

Of course, this assumes we have conveniently named each directory the same as its package name.
