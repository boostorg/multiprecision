=================
CMInstallTargets
=================

-------------------
cm_install_targets
-------------------

.. program:: cm_install_targets

This installs the targets specified. The directories will be installed according to GNUInstallDirs.
It will also install a corresponding cmake package config(which can be found with ``find_package``) to link against the library targets. 

.. option:: TARGETS <target-name>...

The name of the targets to install.

.. option:: INCLUDE <directory>...

Include directories to be installed. It also makes the include directory available for targets to be installed.

.. option:: EXPORT

This specifies an export file. By default, the export file will be named ``${PROJECT_NAME}-targets``.

