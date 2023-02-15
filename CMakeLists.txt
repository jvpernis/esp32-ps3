idf_component_register(
        SRCS
          "src/ps3.c"
          "src/ps3_spp.c"
          "src/ps3_parser.c"
          "src/ps3_l2cap.c"
        REQUIRES "bt" "nvs_flash"
        INCLUDE_DIRS src/include
        PRIV_INCLUDE_DIRS
          $ENV{IDF_PATH}/components/bt/common/include
          $ENV{IDF_PATH}/components/bt/host/bluedroid/common/include
          $ENV{IDF_PATH}/components/bt/host/bluedroid/stack/include
          $ENV{IDF_PATH}/components/bt/host/bluedroid/stack/l2cap/include
          #$ENV{IDF_PATH}/components/bt/host/bluedroid/stack/gap/include
)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/bluetooth/esp_ble_mesh/common_components/button
                         $ENV{IDF_PATH}/examples/bluetooth/esp_ble_mesh/common_components/light_driver
                         $ENV{IDF_PATH}/examples/bluetooth/esp_ble_mesh/common_components/example_init
                         $ENV{IDF_PATH}/examples/bluetooth/esp_ble_mesh/common_components/example_nvs)
