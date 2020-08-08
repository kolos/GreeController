#ifndef GREEDEVICE_H
#define GREEDEVICE_H

#include <Arduino.h>
#include <ESPAsyncUDP.h>

namespace GreeControllerLib {

class Device 
{
	public:
		char mac[13]; // 12 + 1
		char key[17]; // 16 + 1
		char* last_input;
		char* last_query;
		IPAddress ip;

		Device(const char* mac, const char* key, IPAddress ip);
		~Device();
};

} // namespace GreeControllerLib
#endif