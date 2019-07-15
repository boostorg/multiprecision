=========
CMIgnorePackage
=========

------------------
cm_ignore_package
------------------

.. program:: cm_ignore_package

This will ignore a package so that subsequent calls to `find_package` will be treated as found. This is useful in the superproject of integrated builds because it will ingore the ``find_package`` calls to a dependency becaue the targets are already provided by ``add_subdirectory``.

.. option:: NAME

The name of the package to ignore.