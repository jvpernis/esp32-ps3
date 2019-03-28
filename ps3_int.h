#ifndef PS3_INT_H
#define PS3_INT_H

/** Size of the output report buffer for the Dualshock and Navigation controllers */
#define PS3_REPORT_BUFFER_SIZE 48
#define PS3_HID_BUFFER_SIZE    50

/********************************************************************************/
/*                         S H A R E D   T Y P E S                              */
/********************************************************************************/

enum hid_cmd_code {
    hid_cmd_code_set_report   = 0x50,
    hid_cmd_code_type_output  = 0x0f,
    hid_cmd_code_type_feature = 0x03
};

enum hid_cmd_identifier {
    hid_cmd_identifier_ps3_enable  = 0xf4,
    hid_cmd_identifier_ps3_control = 0x01
};


typedef struct {
  uint8_t code;
  uint8_t identifier;
  uint8_t data[PS3_REPORT_BUFFER_SIZE];

} hid_cmd_t;

enum ps3_control_packet_index {
    ps3_control_packet_index_rumble_right_duration = 1,
    ps3_control_packet_index_rumble_right_intensity = 2,
    ps3_control_packet_index_rumble_left_duration = 3,
    ps3_control_packet_index_rumble_left_intensity = 4,

    ps3_control_packet_index_leds = 9
};

enum ps3_led_mask {
    ps3_led_mask_led1 = 1 << 1,
    ps3_led_mask_led2 = 1 << 2,
    ps3_led_mask_led3 = 1 << 3,
    ps3_led_mask_led4 = 1 << 4,
};

/********************************************************************************/
/*                      S H A R E D   V A R I A B L E S                         */
/********************************************************************************/

extern ps3_t ps3;
extern ps3_event_callback_t ps3_event_cb;
extern bool is_connected;


/********************************************************************************/
/*                      P A R S E R   F U N C T I O N S                         */
/********************************************************************************/

void ps3_parse_packet( uint8_t *packet );


/********************************************************************************/
/*                          S P P   F U N C T I O N S                           */
/********************************************************************************/

void ps3_spp_init();


/********************************************************************************/
/*                        L 2 C A P   F U N C T I O N S                         */
/********************************************************************************/

void ps3_l2cap_init_services();
void ps3_l2cap_send_hid( hid_cmd_t *hid_cmd, uint8_t len );

#endif