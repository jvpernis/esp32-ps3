#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "include/ps3.h"
#include "include/ps3_int.h"
#include "esp_log.h"

#define  PS3_TAG "PS3_PARSER"


/********************************************************************************/
/*                            L O C A L    T Y P E S                            */
/********************************************************************************/

enum ps3_packet_index {
    ps3_packet_index_buttons_raw = 12,

    ps3_packet_index_analog_stick_lx = 16,
    ps3_packet_index_analog_stick_ly = 17,
    ps3_packet_index_analog_stick_rx = 18,
    ps3_packet_index_analog_stick_ry = 19,

    ps3_packet_index_analog_button_up = 24,
    ps3_packet_index_analog_button_right = 25,
    ps3_packet_index_analog_button_down = 26,
    ps3_packet_index_analog_button_left = 27,

    ps3_packet_index_analog_button_l2 = 28,
    ps3_packet_index_analog_button_r2 = 29,
    ps3_packet_index_analog_button_l1 = 30,
    ps3_packet_index_analog_button_r1 = 31,

    ps3_packet_index_analog_button_triangle = 32,
    ps3_packet_index_analog_button_circle = 33,
    ps3_packet_index_analog_button_cross = 34,
    ps3_packet_index_analog_button_square = 35,

    ps3_packet_index_status = 39,

    ps3_packet_index_sensor_accelerometer_x = 51,
    ps3_packet_index_sensor_accelerometer_y = 53,
    ps3_packet_index_sensor_accelerometer_z = 55,
    ps3_packet_index_sensor_gyroscope_z = 57
};

enum ps3_button_mask {
    ps3_button_mask_select   = 1 << 0,
    ps3_button_mask_l3       = 1 << 1,
    ps3_button_mask_r3       = 1 << 2,
    ps3_button_mask_start    = 1 << 3,

    ps3_button_mask_up       = 1 << 4,
    ps3_button_mask_right    = 1 << 5,
    ps3_button_mask_down     = 1 << 6,
    ps3_button_mask_left     = 1 << 7,

    ps3_button_mask_l2       = 1 << 8,
    ps3_button_mask_r2       = 1 << 9,
    ps3_button_mask_l1       = 1 << 10,
    ps3_button_mask_r1       = 1 << 11,

    ps3_button_mask_triangle = 1 << 12,
    ps3_button_mask_circle   = 1 << 13,
    ps3_button_mask_cross    = 1 << 14,
    ps3_button_mask_square   = 1 << 15,

    ps3_button_mask_ps       = 1 << 16
};

enum ps3_status_mask {
    ps3_status_mask_rumbling = 0x02,
    ps3_status_mask_bluetooth = 0x04
};


/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/

ps3_sensor_t ps3_parse_packet_sensor( uint8_t *packet );
ps3_status_t ps3_parse_packet_status( uint8_t *packet );
ps3_analog_stick_t ps3_parse_packet_analog_stick( uint8_t *packet );
ps3_analog_button_t ps3_parse_packet_analog_button( uint8_t *packet );
ps3_button_t ps3_parse_packet_buttons( uint8_t *packet );
ps3_event_t ps3_parse_event( ps3_t prev, ps3_t cur );


/********************************************************************************/
/*                         L O C A L    V A R I A B L E S                       */
/********************************************************************************/

static ps3_t ps3;
static ps3_event_callback_t ps3_event_cb = NULL;


/********************************************************************************/
/*                      P U B L I C    F U N C T I O N S                        */
/********************************************************************************/
void ps3_parser_set_event_cb( ps3_event_callback_t cb )
{
    ps3_event_cb = cb;
}

void ps3_parse_packet( uint8_t *packet )
{
    ps3_t prev_ps3 = ps3;

    ps3.button        = ps3_parse_packet_buttons(packet);
    ps3.analog.stick  = ps3_parse_packet_analog_stick(packet);
    ps3.analog.button = ps3_parse_packet_analog_button(packet);
    ps3.sensor        = ps3_parse_packet_sensor(packet);
    ps3.status        = ps3_parse_packet_status(packet);

    ps3_event_t ps3_event = ps3_parse_event( prev_ps3, ps3 );

    ps3_packet_event( ps3, ps3_event );


}


/********************************************************************************/
/*                      L O C A L    F U N C T I O N S                          */
/********************************************************************************/

/******************/
/*    E V E N T   */
/******************/
ps3_event_t ps3_parse_event( ps3_t prev, ps3_t cur )
{
    ps3_event_t ps3_event;

    /* Button down events */
    ps3_event.button_down.select   = !prev.button.select   && cur.button.select;
    ps3_event.button_down.l3       = !prev.button.l3       && cur.button.l3;
    ps3_event.button_down.r3       = !prev.button.r3       && cur.button.r3;
    ps3_event.button_down.start    = !prev.button.start    && cur.button.start;

    ps3_event.button_down.up       = !prev.button.up       && cur.button.up;
    ps3_event.button_down.right    = !prev.button.right    && cur.button.right;
    ps3_event.button_down.down     = !prev.button.down     && cur.button.down;
    ps3_event.button_down.left     = !prev.button.left     && cur.button.left;

    ps3_event.button_down.l2       = !prev.button.l2       && cur.button.l2;
    ps3_event.button_down.r2       = !prev.button.r2       && cur.button.r2;
    ps3_event.button_down.l1       = !prev.button.l1       && cur.button.l1;
    ps3_event.button_down.r1       = !prev.button.r1       && cur.button.r1;

    ps3_event.button_down.triangle = !prev.button.triangle && cur.button.triangle;
    ps3_event.button_down.circle   = !prev.button.circle   && cur.button.circle;
    ps3_event.button_down.cross    = !prev.button.cross    && cur.button.cross;
    ps3_event.button_down.square   = !prev.button.square   && cur.button.square;

    ps3_event.button_down.ps       = !prev.button.ps       && cur.button.ps;

    /* Button up events */
    ps3_event.button_up.select   = prev.button.select   && !cur.button.select;
    ps3_event.button_up.l3       = prev.button.l3       && !cur.button.l3;
    ps3_event.button_up.r3       = prev.button.r3       && !cur.button.r3;
    ps3_event.button_up.start    = prev.button.start    && !cur.button.start;

    ps3_event.button_up.up       = prev.button.up       && !cur.button.up;
    ps3_event.button_up.right    = prev.button.right    && !cur.button.right;
    ps3_event.button_up.down     = prev.button.down     && !cur.button.down;
    ps3_event.button_up.left     = prev.button.left     && !cur.button.left;

    ps3_event.button_up.l2       = prev.button.l2       && !cur.button.l2;
    ps3_event.button_up.r2       = prev.button.r2       && !cur.button.r2;
    ps3_event.button_up.l1       = prev.button.l1       && !cur.button.l1;
    ps3_event.button_up.r1       = prev.button.r1       && !cur.button.r1;

    ps3_event.button_up.triangle = prev.button.triangle && !cur.button.triangle;
    ps3_event.button_up.circle   = prev.button.circle   && !cur.button.circle;
    ps3_event.button_up.cross    = prev.button.cross    && !cur.button.cross;
    ps3_event.button_up.square   = prev.button.square   && !cur.button.square;

    ps3_event.button_up.ps       = prev.button.ps       && !cur.button.ps;

    /* Analog events */
    ps3_event.analog_changed.stick.lx        = cur.analog.stick.lx - prev.analog.stick.lx;
    ps3_event.analog_changed.stick.ly        = cur.analog.stick.ly - prev.analog.stick.ly;
    ps3_event.analog_changed.stick.rx        = cur.analog.stick.rx - prev.analog.stick.rx;
    ps3_event.analog_changed.stick.ry        = cur.analog.stick.ry - prev.analog.stick.ry;

    ps3_event.analog_changed.button.up       = cur.analog.button.up    - prev.analog.button.up;
    ps3_event.analog_changed.button.right    = cur.analog.button.right - prev.analog.button.right;
    ps3_event.analog_changed.button.down     = cur.analog.button.down  - prev.analog.button.down;
    ps3_event.analog_changed.button.left     = cur.analog.button.left  - prev.analog.button.left;

    ps3_event.analog_changed.button.l2       = cur.analog.button.l2 - prev.analog.button.l2;
    ps3_event.analog_changed.button.r2       = cur.analog.button.r2 - prev.analog.button.r2;
    ps3_event.analog_changed.button.l1       = cur.analog.button.l1 - prev.analog.button.l1;
    ps3_event.analog_changed.button.r1       = cur.analog.button.r1 - prev.analog.button.r1;

    ps3_event.analog_changed.button.triangle = cur.analog.button.triangle - prev.analog.button.triangle;
    ps3_event.analog_changed.button.circle   = cur.analog.button.circle   - prev.analog.button.circle;
    ps3_event.analog_changed.button.cross    = cur.analog.button.cross    - prev.analog.button.cross;
    ps3_event.analog_changed.button.square   = cur.analog.button.square   - prev.analog.button.square;

    return ps3_event;
}

/********************/
/*    A N A L O G   */
/********************/
ps3_analog_stick_t ps3_parse_packet_analog_stick( uint8_t *packet )
{
    ps3_analog_stick_t ps3_analog_stick;

    const uint8_t int_offset = 0x80;

    ps3_analog_stick.lx = (int16_t)packet[ps3_packet_index_analog_stick_lx] - int_offset;
    ps3_analog_stick.ly = (int16_t)packet[ps3_packet_index_analog_stick_ly] - int_offset;
    ps3_analog_stick.rx = (int16_t)packet[ps3_packet_index_analog_stick_rx] - int_offset;
    ps3_analog_stick.ry = (int16_t)packet[ps3_packet_index_analog_stick_ry] - int_offset;

    return ps3_analog_stick;
}

ps3_analog_button_t ps3_parse_packet_analog_button( uint8_t *packet )
{
    ps3_analog_button_t ps3_analog_button;

    ps3_analog_button.up       = packet[ps3_packet_index_analog_button_up];
    ps3_analog_button.right    = packet[ps3_packet_index_analog_button_right];
    ps3_analog_button.down     = packet[ps3_packet_index_analog_button_down];
    ps3_analog_button.left     = packet[ps3_packet_index_analog_button_left];

    ps3_analog_button.l2       = packet[ps3_packet_index_analog_button_l2];
    ps3_analog_button.r2       = packet[ps3_packet_index_analog_button_r2];
    ps3_analog_button.l1       = packet[ps3_packet_index_analog_button_l1];
    ps3_analog_button.r1       = packet[ps3_packet_index_analog_button_r1];

    ps3_analog_button.triangle = packet[ps3_packet_index_analog_button_triangle];
    ps3_analog_button.circle   = packet[ps3_packet_index_analog_button_circle];
    ps3_analog_button.cross    = packet[ps3_packet_index_analog_button_cross];
    ps3_analog_button.square   = packet[ps3_packet_index_analog_button_square];

    return ps3_analog_button;
}

/*********************/
/*   B U T T O N S   */
/*********************/

ps3_button_t ps3_parse_packet_buttons( uint8_t *packet )
{
    ps3_button_t ps3_button;
    uint32_t ps3_buttons_raw = *((uint32_t*)&packet[ps3_packet_index_buttons_raw]);

    ps3_button.select   = (ps3_buttons_raw & ps3_button_mask_select)   ? true : false;
    ps3_button.l3       = (ps3_buttons_raw & ps3_button_mask_l3)       ? true : false;
    ps3_button.r3       = (ps3_buttons_raw & ps3_button_mask_r3)       ? true : false;
    ps3_button.start    = (ps3_buttons_raw & ps3_button_mask_start)    ? true : false;

    ps3_button.up       = (ps3_buttons_raw & ps3_button_mask_up)       ? true : false;
    ps3_button.right    = (ps3_buttons_raw & ps3_button_mask_right)    ? true : false;
    ps3_button.down     = (ps3_buttons_raw & ps3_button_mask_down)     ? true : false;
    ps3_button.left     = (ps3_buttons_raw & ps3_button_mask_left)     ? true : false;

    ps3_button.l2       = (ps3_buttons_raw & ps3_button_mask_l2)       ? true : false;
    ps3_button.r2       = (ps3_buttons_raw & ps3_button_mask_r2)       ? true : false;
    ps3_button.l1       = (ps3_buttons_raw & ps3_button_mask_l1)       ? true : false;
    ps3_button.r1       = (ps3_buttons_raw & ps3_button_mask_r1)       ? true : false;

    ps3_button.triangle = (ps3_buttons_raw & ps3_button_mask_triangle) ? true : false;
    ps3_button.circle   = (ps3_buttons_raw & ps3_button_mask_circle)   ? true : false;
    ps3_button.cross    = (ps3_buttons_raw & ps3_button_mask_cross)    ? true : false;
    ps3_button.square   = (ps3_buttons_raw & ps3_button_mask_square)   ? true : false;

    ps3_button.ps       = (ps3_buttons_raw & ps3_button_mask_ps)       ? true : false;

    return ps3_button;
}

/*******************************/
/*   S T A T U S   F L A G S   */
/*******************************/
ps3_status_t ps3_parse_packet_status( uint8_t *packet )
{
    ps3_status_t ps3_status;

    ps3_status.battery    =  packet[ps3_packet_index_status+1];
    ps3_status.charging   =  ps3_status.battery == ps3_status_battery_charging;
    ps3_status.connection = (packet[ps3_packet_index_status+2] & ps3_status_mask_bluetooth) ? ps3_status_connection_bluetooth : ps3_status_connection_usb;
    ps3_status.rumbling   = (packet[ps3_packet_index_status+2] & ps3_status_mask_rumbling) ? false: true;

    return ps3_status;
}

/********************/
/*   S E N S O R S  */
/********************/
ps3_sensor_t ps3_parse_packet_sensor( uint8_t *packet )
{
    ps3_sensor_t ps3_sensor;

    const uint16_t int_offset = 0x200;

    ps3_sensor.accelerometer.x = (packet[ps3_packet_index_sensor_accelerometer_x] << 8) + packet[ps3_packet_index_sensor_accelerometer_x+1] - int_offset;
    ps3_sensor.accelerometer.y = (packet[ps3_packet_index_sensor_accelerometer_y] << 8) + packet[ps3_packet_index_sensor_accelerometer_y+1] - int_offset;
    ps3_sensor.accelerometer.z = (packet[ps3_packet_index_sensor_accelerometer_z] << 8) + packet[ps3_packet_index_sensor_accelerometer_z+1] - int_offset;
    ps3_sensor.gyroscope.z     = (packet[ps3_packet_index_sensor_gyroscope_z]     << 8) + packet[ps3_packet_index_sensor_gyroscope_z+1]     - int_offset;

    return ps3_sensor;

}
