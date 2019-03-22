
Sources
==============
http://wiki.ros.org/ps3joy

https://github.com/felis/USB_Host_Shield_2.0/wiki/PS3-Information#Bluetooth

https://github.com/ros-drivers/joystick_drivers/blob/master/ps3joy/scripts/ps3joy_node.py



Examples:
==============
    if ( ps3.status.connection == ps3_status_connection_bluetooth ) print("The controller is currently connected via bluetooth");
    if ( ps3.status.battery >= ps3_status_battery_high ) print("The controller still has sufficient battery charge");
    if ( ps3.status.charging ) print("Controller is charging");
    if ( ps3.button.triangle ) print("Pressing the trangle button");
    if ( ps3.analog.stick.lx < 0 ) print("Pulling analog stick to the left");
    if ( event.button_down.cross ) print("The user started pressing the X button");
    if ( event.button_up.cross ) print("The user released the X button");
    if ( event.analog_changed.stick.lx ) print("The user has moved the left stick sideways");