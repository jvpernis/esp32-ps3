#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ps3.h"
#include "ps3_int.h"


/********************************************************************************/
/*                      P U B L I C    F U N C T I O N S                        */
/********************************************************************************/

/*******************************************************************************
**
** Function         ps3Init
**
** Description      This initializes the bluetooth services to listen
**                  for an incoming PS3 controller connection.
**
**
** Returns          void
**
*******************************************************************************/
void ps3Init()
{
    ps3_spp_init();
    ps3_l2cap_init_services();
}


/*******************************************************************************
**
** Function         ps3IsConnected
**
** Description      This returns whether a PS3 controller is connected, based
**                  on whether a successful handshake has taken place.
**
**
** Returns          bool
**
*******************************************************************************/
bool ps3IsConnected()
{
    return is_connected;
}


/*******************************************************************************
**
** Function         ps3Enable
**
** Description      This triggers the PS3 controller to start continually
**                  sending its data.
**
**
** Returns          void
**
*******************************************************************************/
void ps3Enable()
{
    uint8_t hid_cmd_payload_ps3_enable[] = { 0x42, 0x03, 0x00, 0x00 };

    uint16_t len = sizeof(hid_cmd_payload_ps3_enable);
    hid_cmd_t hid_cmd;

    hid_cmd.code = hid_cmd_code_set_report | hid_cmd_code_type_feature;
    hid_cmd.identifier = hid_cmd_identifier_ps3_enable;

    memcpy( hid_cmd.data, hid_cmd_payload_ps3_enable, len);

    ps3_l2cap_send_hid( &hid_cmd, len );
}

/*******************************************************************************
**
** Function         ps3Cmd
**
** Description      Send a command to the PS3 controller.
**
**
** Returns          void
**
*******************************************************************************/
void ps3Cmd( ps3_cmd_t cmd )
{
    uint16_t len = sizeof(cmd);
    hid_cmd_t hid_cmd = { .data = {0} };

    hid_cmd.code = hid_cmd_code_set_report | hid_cmd_code_type_output;
    hid_cmd.identifier = hid_cmd_identifier_ps3_control;

    hid_cmd.data[ps3_control_packet_index_rumble_right_duration]  = cmd.rumble_right_duration;
    hid_cmd.data[ps3_control_packet_index_rumble_right_intensity] = cmd.rumble_right_intensity;
    hid_cmd.data[ps3_control_packet_index_rumble_left_duration]   = cmd.rumble_left_duration;
    hid_cmd.data[ps3_control_packet_index_rumble_left_intensity]  = cmd.rumble_left_intensity;

    hid_cmd.data[ps3_control_packet_index_leds] = 0;
    if (cmd.led1) hid_cmd.data[ps3_control_packet_index_leds] |= ps3_led_mask_led1;
    if (cmd.led2) hid_cmd.data[ps3_control_packet_index_leds] |= ps3_led_mask_led2;
    if (cmd.led3) hid_cmd.data[ps3_control_packet_index_leds] |= ps3_led_mask_led3;
    if (cmd.led4) hid_cmd.data[ps3_control_packet_index_leds] |= ps3_led_mask_led4;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: replace below by continuing the above
    uint8_t hid_cmd_payload[PS3_HID_BUFFER_SIZE]  = {0};
    uint8_t hid_cmd_payload_ps3_enable[] = { 0x52, 0x01, 0x00, 0x5f, 0xff, 0x5f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01 };
    memcpy( hid_cmd_payload, hid_cmd_payload_ps3_enable, sizeof(hid_cmd_payload_ps3_enable));
    len = PS3_HID_BUFFER_SIZE;
    memcpy( hid_cmd.data, hid_cmd_payload, len);

    ps3_l2cap_send_hid( &hid_cmd, len );
}


/*******************************************************************************
**
** Function         ps3SetLed
**
** Description      Sets one of the LEDs on the PS3 controller.
**
**
** Returns          void
**
*******************************************************************************/
void ps3SetLed( uint8_t led )
{
    //TODO
}

/*******************************************************************************
**
** Function         ps3SetEventCallback
**
** Description      Registers a callback for receiving PS3 controller events
**
**
** Returns          void
**
*******************************************************************************/
void ps3SetEventCallback( ps3_event_callback_t cb )
{
    ps3_event_cb = cb;
}


