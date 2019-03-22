#
# Component Makefile
#

COMPONENT_EXTRA_INCLUDES +=     $(IDF_PATH)/components/bt/bluedroid/common/include/           \
                                $(IDF_PATH)/components/bt/bluedroid/stack/include             \
                                $(IDF_PATH)/components/bt/bluedroid/stack/gap/include         \
                                $(IDF_PATH)/components/bt/bluedroid/stack/l2cap/include       \
								$(IDF_PATH)/components/bt/bluedroid/osi/include

COMPONENT_DEPENDS := bt