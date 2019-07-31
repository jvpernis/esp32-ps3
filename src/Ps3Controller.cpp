#include "Ps3Controller.h"

extern "C" {
#include  "include/ps3.h"
}


Ps3Controller::Ps3Controller()
{

}


bool Ps3Controller::begin()
{

}


bool Ps3Controller::begin(char *mac)
{

}


bool Ps3Controller::end()
{

}


bool Ps3Controller::isConnected()
{

}


void Ps3Controller::setLed(int led)
{

}


void Ps3Controller::attach(callback_t callback)
{

}


#if !defined(NO_GLOBAL_INSTANCES)
Ps3Controller Ps3;
#endif
