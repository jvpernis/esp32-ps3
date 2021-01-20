#include "Ps3Controller.h"

#include <esp_bt_main.h>
#include <esp_bt_defs.h>

extern "C" {
#include  "esp_bt_device.h"
#include  "include/ps3.h"
}


#define ESP_BD_ADDR_HEX_STR        "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"
#define ESP_BD_ADDR_HEX_ARR(addr)   addr[0],  addr[1],  addr[2],  addr[3],  addr[4],  addr[5]
#define ESP_BD_ADDR_HEX_PTR(addr)  &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]


Ps3Controller::Ps3Controller()
{

}


bool Ps3Controller::begin()
{
    ps3SetEventObjectCallback(this, &Ps3Controller::_event_callback);
    ps3SetConnectionObjectCallback(this, &Ps3Controller::_connection_callback);

    if(!btStarted() && !btStart()){
        log_e("btStart failed");
        return false;
    }

    esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
    if(bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED){
        if (esp_bluedroid_init()) {
            log_e("esp_bluedroid_init failed");
            return false;
        }
    }

    if(bt_state != ESP_BLUEDROID_STATUS_ENABLED){
        if (esp_bluedroid_enable()) {
            log_e("esp_bluedroid_enable failed");
            return false;
        }
    }

    ps3Init();
    return true;

}


bool Ps3Controller::begin(const char *mac)
{
    esp_bd_addr_t addr;

    if (sscanf(mac, ESP_BD_ADDR_HEX_STR, ESP_BD_ADDR_HEX_PTR(addr)) != ESP_BD_ADDR_LEN){
        log_e("Could not convert %s\n to a MAC address", mac);
        return false;
    }

    ps3SetBluetoothMacAddress( addr );

    return begin();

}


bool Ps3Controller::end()
{
    ps3Deinit();
    return true;
}


String Ps3Controller::getAddress() {
    String address = "";

    if (btStarted()) {
        char mac[18];
        const uint8_t* addr = esp_bt_dev_get_address();

        sprintf(mac, ESP_BD_ADDR_STR, ESP_BD_ADDR_HEX_ARR(addr));

        address = String(mac);
    }

    return address;
}


bool Ps3Controller::isConnected()
{
    return ps3IsConnected();

}


void Ps3Controller::setPlayer(int player)
{
    this->player = player;
    ps3SetLed(player);
}


void Ps3Controller::setRumble(float intensity, int duration) {

    const float int_min = 0.0;
    const float int_max = 100.0;

    const int dur_min = 0;
    const int dur_max = 5000;

    uint8_t raw_intensity = map(constrain(intensity, int_min, int_max), int_min, int_max, 0, 255);
    uint8_t raw_duration = map(constrain(duration, dur_min, dur_max), dur_min, dur_max, 0, 254);

    if (duration == -1) {
        raw_duration = 255;
    }

    ps3_cmd_t cmd = {};

    cmd.rumble_right_intensity = raw_intensity;
    cmd.rumble_left_intensity = raw_intensity;

    cmd.rumble_right_duration = raw_duration;
    cmd.rumble_left_duration = raw_duration;

    ps3SetLedCmd(&cmd, this->player);
    ps3Cmd(cmd);

}


void Ps3Controller::attach(callback_t callback)
{
    _callback_event = callback;

}


void Ps3Controller::attachOnConnect(callback_t callback)
{
    _callback_connect = callback;

}

void Ps3Controller::attachOnDisconnect(callback_t callback)
{
    _callback_disconnect = callback;

}


void Ps3Controller::_event_callback(void *object, ps3_t data, ps3_event_t event)
{
    Ps3Controller* This = (Ps3Controller*) object;

    memcpy(&This->data, &data, sizeof(ps3_t));
    memcpy(&This->event, &event, sizeof(ps3_event_t));

    if (This->_callback_event){
        This->_callback_event();
    }
}


void Ps3Controller::_connection_callback(void *object, uint8_t is_connected)
{
    Ps3Controller* This = (Ps3Controller*) object;

    if (is_connected)
    {
        // Set LED1 by default
        This->setPlayer(1);

        if (This->_callback_connect){
            This->_callback_connect();
        }
    }else
    {
        if (This->_callback_disconnect){
            This->_callback_disconnect();
        }
    }

}

#if !defined(NO_GLOBAL_INSTANCES)
Ps3Controller Ps3;
#endif
