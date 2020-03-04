#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS += ./
COMPONENT_ADD_INCLUDEDIRS += ./../app
COMPONENT_ADD_INCLUDEDIRS += ./../hal_driver
							
COMPONENT_SRCDIRS += ./
COMPONENT_SRCDIRS += ./../app
COMPONENT_SRCDIRS += ./../hal_driver

EXTRA_CFLAGS = -Wno-error