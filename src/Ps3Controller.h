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
        bool begin(const char *mac);
        bool end();

        String getAddress();

        bool isConnected();

        void setPlayer(int player);
        void setRumble(float intensity, int duration = -1);

        void attach(callback_t callback);
        void attachOnConnect(callback_t callback);
        void attachOnDisconnect(callback_t callback);

    private:
        static void _event_callback(void *object, ps3_t data, ps3_event_t event);
        static void _connection_callback(void *object, uint8_t is_connected);

        int player;

        callback_t _callback_event = nullptr;
        callback_t _callback_connect = nullptr;
        callback_t _callback_disconnect = nullptr;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern Ps3Controller Ps3;
#endif

#endif
