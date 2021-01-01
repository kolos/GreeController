#ifndef GREEDEVICE_H
#define GREEDEVICE_H

#include <Arduino.h>
#include <ESPAsyncUDP.h>

namespace GreeControllerLib {

class Device 
{
	public:
		char* mac;
		char* key;
		char* last_input;
		char* last_query;
		char* last_packed_status;
		char* last_status;
		IPAddress ip;

		Device(const char* mac, const char* key, IPAddress ip);
		~Device();
};

} // namespace GreeControllerLib
#endif