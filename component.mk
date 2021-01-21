#
# Component Makefile
#

COMPONENT_SRCDIRS := src
COMPONENT_ADD_INCLUDEDIRS := src/include

COMPONENT_OBJS := src/ps3.o src/ps3_spp.o src/ps3_parser.o src/ps3_l2cap.o

COMPONENT_EXTRA_INCLUDES +=     $(IDF_PATH)/components/bt/common/include/                     \
                                $(IDF_PATH)/components/bt/host/bluedroid/common/include/      \
                                $(IDF_PATH)/components/bt/host/bluedroid/stack/include        \
                                $(IDF_PATH)/components/bt/host/bluedroid/stack/l2cap/include  \
                                $(IDF_PATH)/components/bt/host/bluedroid/osi/include

# Legacy bluedroid location include paths
# i.e. ESP-IDF prior to revision 21165edf41813482bd1d0a8183969854d1011f24
COMPONENT_EXTRA_INCLUDES +=     $(IDF_PATH)/components/bt/bluedroid/common/include/           \
                                $(IDF_PATH)/components/bt/bluedroid/stack/include             \
                                $(IDF_PATH)/components/bt/bluedroid/stack/l2cap/include       \
								$(IDF_PATH)/components/bt/bluedroid/osi/include

COMPONENT_DEPENDS := bt
