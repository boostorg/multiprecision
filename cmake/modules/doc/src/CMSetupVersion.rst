===============
CMSetupVersion
===============

-----------------
cm_setup_version
-----------------

.. program:: cm_setup_version

This sets up the project version by setting these version variables::

    PROJECT_VERSION, ${PROJECT_NAME}_VERSION
    PROJECT_VERSION_MAJOR, ${PROJECT_NAME}_VERSION_MAJOR
    PROJECT_VERSION_MINOR, ${PROJECT_NAME}_VERSION_MINOR
    PROJECT_VERSION_PATCH, ${PROJECT_NAME}_VERSION_PATCH

.. option:: VERSION <major>.<minor>.<patch>

This is the version to be set.

.. option:: GENERATE_HEADER <header-name>

This is a header which will be generated with defines for the version number.

.. option:: PREFIX <identifier>

By default, the upper case of the project name is used as a prefix for the version macros that are defined in the generated header: ``${PREFIX}_VERSION_MAJOR``, ``${PREFIX}_VERSION_MINOR``, ``${PREFIX}_VERSION_PATCH``, and ``${PREFIX}_VERSION``. The ``PREFIX`` option allows overriding the prefix name used for the macros. 

.. option:: PARSE_HEADER <header-name>

Rather than set a version and generate a header, this will parse a header with macros that define the version, and then use those values to set the version for the project.

