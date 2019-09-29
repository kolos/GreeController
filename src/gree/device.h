#include <Arduino.h>
#include <ESPAsyncUDP.h>

class Device 
{
	public:
		char* mac;
		char* key;
		IPAddress ip;

		Device(const char* mac, const char* key, IPAddress ip);
		~Device();
};