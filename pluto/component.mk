#
# Component Makefile
#
LIB_PLUTOSTACK_PATH += $(IDF_PATH)/../pluto/libpluto_stack.a
COMPONENT_ADD_LDFLAGS := $(LIB_PLUTOSTACK_PATH) -lpluto

COMPONENT_ADD_INCLUDEDIRS := ./ \
                            ./../eloop      \
                            ./../eloop/core \
                            ./../eloop/platform \
                             ./aps \
                             ./ATC \
                             ./efs \
                             ./platform \
                             ./socket \
                             ./update \
                             ./zigbee   

COMPONENT_SRCDIRS :=         ./ \
                            ./../eloop/core \
                            ./../eloop/platform \
                             ./aps \
                             ./ATC \
                             ./efs \
                             ./platform \
                             ./socket \
                             ./update \
                             ./zigbee   


