#ifndef PS3_H
#define PS3_H


/********************************************************************************/
/*                                  T Y P E S                                   */
/********************************************************************************/

/********************/
/*    A N A L O G   */
/********************/

typedef struct {
    int8_t lx;
    int8_t ly;
    int8_t rx;
    int8_t ry;
} ps3_analog_stick_t;

typedef struct {
    uint8_t up;
    uint8_t right;
    uint8_t down;
    uint8_t left;

    uint8_t l2;
    uint8_t r2;
    uint8_t l1;
    uint8_t r1;

    uint8_t triangle;
    uint8_t circle;
    uint8_t cross;
    uint8_t square;
} ps3_analog_button_t;

typedef struct {
    ps3_analog_stick_t stick;
    ps3_analog_button_t button;
} ps3_analog_t;


/*********************/
/*   B U T T O N S   */
/*********************/

typedef struct {
    uint8_t select   : 1;
    uint8_t l3       : 1;
    uint8_t r3       : 1;
    uint8_t start    : 1;

    uint8_t up       : 1;
    uint8_t right    : 1;
    uint8_t down     : 1;
    uint8_t left     : 1;

    uint8_t l2       : 1;
    uint8_t r2       : 1;
    uint8_t l1       : 1;
    uint8_t r1       : 1;

    uint8_t triangle : 1;
    uint8_t circle   : 1;
    uint8_t cross    : 1;
    uint8_t square   : 1;

    uint8_t ps       : 1;
} ps3_button_t;


/*******************************/
/*   S T A T U S   F L A G S   */
/*******************************/

enum ps3_status_battery {
    ps3_status_battery_shutdown = 0x01,
    ps3_status_battery_dying    = 0x02,
    ps3_status_battery_low      = 0x03,
    ps3_status_battery_high     = 0x04,
    ps3_status_battery_full     = 0x05,
    ps3_status_battery_charging = 0xEE
};

enum ps3_status_connection {
    ps3_status_connection_usb,
    ps3_status_connection_bluetooth
};

typedef struct {
    enum ps3_status_battery battery;
    enum ps3_status_connection connection;
    uint8_t charging : 1;
    uint8_t rumbling : 1;
} ps3_status_t;


/********************/
/*   S E N S O R S  */
/********************/

typedef struct {
    int16_t z;
} ps3_sensor_gyroscope_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} ps3_sensor_accelerometer_t;

typedef struct {
    ps3_sensor_accelerometer_t accelerometer;
    ps3_sensor_gyroscope_t gyroscope;
} ps3_sensor_t;


/*******************/
/*    O T H E R    */
/*******************/

typedef struct {
    /* Rumble control */
    uint8_t rumble_right_duration;
    uint8_t rumble_right_intensity;
    uint8_t rumble_left_duration;
    uint8_t rumble_left_intensity;

    /* LED control */
    uint8_t led1 : 1;
    uint8_t led2 : 1;
    uint8_t led3 : 1;
    uint8_t led4 : 1;
} ps3_cmd_t;

typedef struct {
    ps3_button_t button_down;
    ps3_button_t button_up;
    ps3_analog_t analog_changed;
} ps3_event_t;

typedef struct {
    ps3_analog_t analog;
    ps3_button_t button;
    ps3_status_t status;
    ps3_sensor_t sensor;

} ps3_t;


/***************************/
/*    C A L L B A C K S    */
/***************************/

typedef void(*ps3_connection_callback_t)( uint8_t is_connected );
typedef void(*ps3_connection_object_callback_t)( void *object, uint8_t is_connected );

typedef void(*ps3_event_callback_t)( ps3_t ps3, ps3_event_t event );
typedef void(*ps3_event_object_callback_t)( void *object, ps3_t ps3, ps3_event_t event );


/********************************************************************************/
/*                             F U N C T I O N S                                */
/********************************************************************************/

bool ps3IsConnected();
void ps3Init();
void ps3Deinit();
void ps3Enable();
void ps3Cmd( ps3_cmd_t ps3_cmd );
void ps3SetConnectionCallback( ps3_connection_callback_t cb );
void ps3SetConnectionObjectCallback( void *object, ps3_connection_object_callback_t cb );
void ps3SetEventCallback( ps3_event_callback_t cb );
void ps3SetEventObjectCallback( void *object, ps3_event_object_callback_t cb );
void ps3SetLed( uint8_t player );
void ps3SetLedCmd( ps3_cmd_t *cmd, uint8_t player );
void ps3SetBluetoothMacAddress( const uint8_t *mac );


#endif
