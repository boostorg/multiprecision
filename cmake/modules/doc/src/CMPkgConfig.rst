============
CMPkgConfig
============

---------------------------
cm_generate_pkgconfig_file
---------------------------

.. program:: cm_generate_pkgconfig_file

This will generate a simple pkgconfig file.

.. option:: NAME <name>

This is the name of the pkgconfig module.

.. option:: LIB_DIR <directory>

This is the directory where the library is linked to. This defaults to ``${CMAKE_INSTALL_LIBDIR}``.

.. option:: INCLUDE_DIR <directory>

This is the include directory where the header file are installed. This defaults to ``${CMAKE_INSTALL_INCLUDEDIR}``.

.. option:: DESCRIPTION <text>

A description about the library.

.. option:: TARGETS <targets>...

The library targets to link.

.. option:: CFLAGS <flags>...

Additionaly, compiler flags.

.. option:: LIBS <library flags>...

Additional libraries to be linked.

.. option:: REQUIRES <packages>...

List of other pkgconfig packages that this module depends on.

------------------
cm_auto_pkgconfig
------------------

.. program:: cm_auto_pkgconfig

This will auto generate pkgconfig from a given target. All the compiler and linker flags come from the target.

.. option:: NAME <name>

This is the name of the pkgconfig module. By default, this will use the project name.

.. option:: TARGET <TARGET>

This is the target which will be used to set the various pkgconfig fields.
