#include <Arduino.h>
#include <ESPAsyncUDP.h>

class Device 
{
  private:
    const char* mac;
    const char* key;
    IPAddress ip;
  public:
    Device(const char* mac, const char* key, IPAddress ip);
};