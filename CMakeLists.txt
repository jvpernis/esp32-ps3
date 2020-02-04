idf_component_register(SRCS "src/ps3.c"
                            "src/ps3_gap.c"
                            "src/ps3_parser.c"
                            "src/ps3_spp.c"
                    REQUIRES nvs_flash bt
                    PRIV_REQUIRES bt
                    INCLUDE_DIRS src/include
                    PRIV_INCLUDE_DIRS $ENV{IDF_PATH}/components/bt/common/include
                                      $ENV{IDF_PATH}/components/bt/host/bluedroid/common/include
                                      $ENV{IDF_PATH}/components/bt/host/bluedroid/stack/include
                                      $ENV{IDF_PATH}/components/bt/host/bluedroid/stack/gap/include)
