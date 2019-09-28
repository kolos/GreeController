#include <ESPAsyncUDP.h>
#include <ArduinoJson.h>
#include <vector>

#include "device.h"
#include "greepacker.h"

#define GREE_PORT 7000
#define BASE_KEY "a3K8Bx%2r8Y7#xDh"

class Devices
{
  private:
    AsyncUDP udp;
    std::vector<Device> devices;
	const char* base_key = BASE_KEY;
    
    void handleHandshakePacket(IPAddress remoteIP, const JsonObject& root);
    void packetHandler(AsyncUDPPacket packet);
    void addDevice(const char* mac, const char* key, IPAddress remoteIP);
    void sendBindingRequest(IPAddress remoteIP, const char* mac);
  public:
    void listen();
    void scan();
};