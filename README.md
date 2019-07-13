
Description
==============
Control your ESP32 projects with a PS3 controller!

This library is meant to be used with Espressif's [ESP-IDF](https://github.com/espressif/esp-idf) IoT Development Framework.


Pairing the PS3 Controller:
==============
When a PS3 controller is 'paired' to a PS3 console, it just means that it has stored the console's Bluetooth MAC address, which is the only device the controller will connect to. Usually, this pairing happens when you connect the controller to the PS3 console using a USB cable, and press the PS button. This initiates writing the console's MAC address to the controller.

Therefore, if you want to connect your PS3 controller to the ESP32, you either need to figure out what the Bluetooth MAC address of your PS3 console is and set the ESP32's address to it, or change the MAC address stored in the PS3 controller.

Whichever path you choose, you're going to need a tool to read and/or write the currently paired MAC address from the PS3 controller. I used [SixaxisPairTool](https://dancingpixelstudios.com/sixaxis-controller/sixaxispairtool/) for this, but you can try using [sixaxispairer](https://github.com/user-none/sixaxispairer) as well, if open source is important to you.

If you opted to change the ESP32's MAC address, you'll need to include this snippet in your code **before** calling ```ps3Init()```, where the MAC address should match with the one stored on the PS3 controller:
```c
uint8_t new_mac[8] = {0x01,0x02,0x03,0x04,0x05,0x06};
ps3SetBluetoothMacAddress(new_mac);
```

Getting Started
==============

### Installing the library ###

There are two options to install this library: either install it globally in the ESP-IDF folder so all of your projects can make use of it, or install it locally in your project folder.

Go to the root of either, and clone this repository by running:

`git clone https://github.com/jvpernis/esp32-ps3.git components/ps3`

In case you are using git to manage your project, you can also add this library to your repository by running:

`git submodule add https://github.com/jvpernis/esp32-ps3.git components/ps3`

### Configuring your project ###
Make sure you have configured your project correctly in order to use your PS3 controller.
In your project folder, run `make menuconfig` and configure your project with the following steps:
- Navigate to `Component config  --->` and press <kbd>Enter</kbd> to open the component config menu.
- Navigate to `Bluetooth  --->` and press <kbd>Enter</kbd> to open the Bluetooth config menu.
- Select `[] Bluetooth` and press <kbd>Y</kbd> to enable Bluetooth
- Navigate to `Bluetooth controller  --->` and press <kbd>Enter</kbd> to open its menu.
- Navigate to `Bluetooth controller mode` and press <kbd>Enter</kbd> to show the controller mode options.
- Select `BR/EDR Only` and press <kbd>Enter</kbd> to confirm the choice.
- Press <kbd>E</kbd> to go back to the Bluetooth menu.
- If it exists, navigate to `Bluetooth Host` and press <kbd>Enter</kbd> to show the host options.
- Select `Bluedroid - Dual-mode` and press <kbd>Enter</kbd> to confirm the choice.
- Navigate to `[] Bluedroid Enable  --->` and press <kbd>Y</kbd> to enable Bluedroid if it isn't already, then press <kbd>Enter</kbd> to open its menu.
- Navigate to `Classic Bluetooth` and press <kbd>Y</kbd> to enable it
- Navigate to `SPP` and press <kbd>Y</kbd> to enable it
- Navigate to `Secure Simple Pairing` and press <kbd>Y</kbd> to enable it if it isn't already
- Press <kbd>S</kbd> to save the configuration.


### Using the library ###
In order to use this library, you just need to set an event callback, call the initialisation function, and, optionally, wait for the PS3 controller to be connected:
```c
#include "ps3.h"
#include "freertos/task.h"

ps3SetEventCallback(controller_event_cb);
ps3Init();

while (!ps3IsConnected()){
    // Prevent the Task Watchdog from triggering
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
```

Your event callback will have to look like this:
```c
void controller_event_cb( ps3_t ps3, ps3_event_t event )
{
    // Event handling here...
}
```


### Examples
```c
if ( ps3.status.battery >= ps3_status_battery_high )
    print("The controller still has sufficient battery charge");

if ( ps3.status.charging )
    print("Controller is charging");

if ( ps3.button.triangle )
    print("Currently pressing the trangle button");

if ( ps3.analog.stick.lx < 0 )
    print("Currently pulling analog stick to the left");

if ( event.button_down.cross )
    print("The user started pressing the X button");

if ( event.button_up.cross )
    print("The user released the X button");

if ( event.analog_changed.stick.lx )
    print("The user has moved the left stick sideways");
```

Troubleshooting
==============

### The component gives compilation errors ###
This project uses ESP-IDF internal API's in order to implement the PS3 controller functionality. This has a drawback of being susceptible to being suddenly broken. To remedy this, a compatibility config menu has been added so you can select older ESP-IDF versions to work with.

In your project folder, run `make menuconfig` and configure your project with the following steps:
- Navigate to `Component config  --->` and press <kbd>Enter</kbd> to open the component config menu.
- Navigate to `PS3  --->` and press <kbd>Enter</kbd> to open the Bluetooth config menu.
- Navigate to `Framework compatibility` and press <kbd>Enter</kbd> to show the compatibility options.
- Select the compatibility suitable to your ESP-IDF version

If you selected the `Latest stable release` or `Latest master revision` option and you are still getting compiler errors, please [create an issue](https://github.com/jvpernis/esp32-ps3/issues/new).

If you are unsure which master branch revision you should take, figure out what the commit date is of your ESP-IDF version (by running `git show`), and look at the help text of each revision listed in the `Framework compatibility` configuration option to know their revision date.


Sources
==============
I've had tremendous help developing this library by reading these sources:

- http://wiki.ros.org/ps3joy

- https://github.com/ros-drivers/joystick_drivers/blob/master/ps3joy/scripts/ps3joy_node.py

- https://github.com/felis/USB_Host_Shield_2.0/wiki/PS3-Information#Bluetooth
