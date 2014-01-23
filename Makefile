# This variable should contain a space separated list of all
# the directories containing buildable applications (usually
# prefixed with the app_ prefix)
#
# If the variable is set to "all" then all directories that start with app_
# are built.
BUILD_SUBDIRS = app_ctrlproto-test app_linux_ctrlproto-csp-example app_linux_ctrlproto-cst-example app_linux_ctrlproto-csv-example app_linux_ctrlproto-master-example app_linux_ctrlproto-ppm-example app_linux_ctrlproto-ptm-example app_linux_ctrlproto-pvm-example lib_linux_ctrlproto lib_linux_motor_drive

XMOS_MAKE_PATH ?= ..
include $(XMOS_MAKE_PATH)/xcommon/module_xcommon/build/Makefile.toplevel
