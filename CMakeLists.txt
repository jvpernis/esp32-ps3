idf_component_register(SRCS "ps3.c"
                            "ps3_gap.c"
                            "ps3_parser.c"
                            "ps3_spp.c"
                    REQUIRES nvs_flash bt
                    PRIV_REQUIRES bt
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS $ENV{IDF_PATH}/components/bt/common/include
                                      $ENV{IDF_PATH}/components/bt/host/bluedroid/common/include
                                      $ENV{IDF_PATH}/components/bt/host/bluedroid/stack/include
                                      $ENV{IDF_PATH}/components/bt/host/bluedroid/stack/gap/include)
