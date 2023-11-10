#ifndef GREEDEVICE_H
#define GREEDEVICE_H

#include <Arduino.h>

namespace GreeControllerLib {

class Device 
{
	public:
		char* mac;
		char* key;
		IPAddress ip;

		Device(const char* mac, const char* key, IPAddress ip);
		~Device();
};

} // namespace GreeControllerLib
#endif