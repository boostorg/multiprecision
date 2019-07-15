=========
CMExport
=========

---------------
cm_auto_export
---------------

.. program:: cm_auto_export

This generates a simple cmake config file that includes the exported targets.

.. option:: EXPORT

This specifies an export file. By default, the export file will be named ``${PROJECT_NAME}-targets``.

.. option:: NAMESPACE <namespace>

This is the namespace to add to the targets that are exported.

.. option:: COMPATIBILITY <compatibility>

This uses the version compatibility specified by cmake version config.

.. option:: NAME <name>

This is the name to use for the package config file. By default, this uses the project name, but this parameter can override it.

.. option:: TARGETS <target>...

These include the targets to be exported.
