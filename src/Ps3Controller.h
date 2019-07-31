#ifndef Ps3Controller_h
#define Ps3Controller_h

#include <inttypes.h>

#include "Arduino.h"

extern "C" {
#include  "include/ps3.h"
}


class Ps3Controller
{
    public:
        typedef void(*callback_t)();

        ps3_t data;
        ps3_event_t event;

        Ps3Controller();

        bool begin();
        bool begin(char *mac);
        bool end();

        bool isConnected();

        void setLed(int led);

        void attach(callback_t callback);

    private:
        callback_t _callback = nullptr;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern Ps3Controller Ps3;
#endif

#endif
