#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "include/ps3.h"
#include "include/ps3_int.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#define PS3_TAG "PS3_SPP"
#define PS3_DEVICE_NAME "PS3 Host"
#define PS3_SERVER_NAME "PS3_SERVER"

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
static void ps3_spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);


/********************************************************************************/
/*                      P U B L I C    F U N C T I O N S                        */
/********************************************************************************/

/*******************************************************************************
**
** Function         ps3_spp_init
**
** Description      Initialise the SPP server to allow to be connected to
**
** Returns          void
**
*******************************************************************************/
void ps3_spp_init()
{
    esp_err_t ret;

#ifndef ARDUINO_ARCH_ESP32
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(BT_MODE)) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
#endif

    if ((ret = esp_spp_register_callback(ps3_spp_callback)) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(ESP_SPP_MODE_CB)) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
}


/*******************************************************************************
**
** Function         ps3_spp_deinit
**
** Description      Deinitialise the SPP server
**
** Returns          void
**
*******************************************************************************/
void ps3_spp_deinit()
{

    esp_err_t ret;

    if ((ret = esp_spp_deinit()) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s spp deinit failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#ifndef ARDUINO_ARCH_ESP32
    if ((ret = esp_bluedroid_disable()) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s disable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_deinit()) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s deinitialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_disable()) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s disable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_deinit()) != ESP_OK) {
        ESP_LOGE(PS3_TAG, "%s deinitialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
#endif
}



/********************************************************************************/
/*                      L O C A L    F U N C T I O N S                          */
/********************************************************************************/

/*******************************************************************************
**
** Function         ps3_spp_callback
**
** Description      Callback for SPP events, only used for the init event to
**                  configure the SPP server
**
** Returns          void
**
*******************************************************************************/
static void ps3_spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (event == ESP_SPP_INIT_EVT) {
        ESP_LOGI(PS3_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(PS3_DEVICE_NAME);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
#else
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE);
#endif

        esp_spp_start_srv(ESP_SPP_SEC_NONE,ESP_SPP_ROLE_SLAVE, 0, PS3_SERVER_NAME);
    }
}
