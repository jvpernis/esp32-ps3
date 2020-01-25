#include <Ps3Controller.h>

void setup()
{
    Serial.begin(115200);
    Ps3.begin("01:02:03:04:05:06");
    Serial.println("Ready.");
}

void loop()
{
  if (Ps3.isConnected()){
    Serial.println("Connected!");
  }

  delay(3000);
}
