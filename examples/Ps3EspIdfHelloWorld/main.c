#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "ps3.h"
#include "nvs_flash.h"

void controller_event_cb(ps3_t ps3, ps3_event_t event)
{
    if (ps3.status.battery >= ps3_status_battery_high)
        printf("batt ok\n");

    if (ps3.status.charging)
        printf("Controller is charging\n");

    if (ps3.button.triangle)
        printf("Currently pressing the trangle button\n");

    if (ps3.analog.stick.lx < 0)
        printf("stick left\n");

    if (event.button_down.cross)
        printf("The user started pressing the X button\n");

    if (event.button_up.cross)
        printf("The user released the X button\n");

    if (event.analog_changed.stick.lx)
        printf("The user has moved the left stick sideways\n");
}

void app_main(void)
{
    printf("Hello world!\n");

    ESP_ERROR_CHECK(nvs_flash_init());
    ps3SetEventCallback(controller_event_cb);
    uint8_t mac[6] = { 0x44, 0xd8, 0x32, 0xbe, 0x1a, 0xc6 };
    ps3SetBluetoothMacAddress(mac);
    ps3Init();

    while (!ps3IsConnected()) {
        // Prevent the Task Watchdog from triggering
        vTaskDelay(100 / portTICK_PERIOD_MS);
        printf("waiting\n");
    }
}
