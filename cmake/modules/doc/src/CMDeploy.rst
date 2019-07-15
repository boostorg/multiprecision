=========
CMDeploy
=========

----------
cm_deploy
----------

.. program:: cm_deploy

This will install targets, as well as generate package configuration for both cmake and pkgconfig.

.. option:: TARGETS <target-name>...

The name of the targets to deploy.

.. option:: INCLUDE <directory>...

Include directories to be installed. It also makes the include directory available for targets to be installed.

.. option:: NAMESPACE <namespace>

This is the namespace to add to the targets that are exported.

.. option:: COMPATIBILITY <compatibility>

This uses the version compatibility specified by cmake version config.

