#include "GreeDevice.h"

using namespace GreeControllerLib;

Device::Device(const char* _mac, const char* _key, IPAddress _ip): ip(_ip) {
	mac = strndup(_mac, 12);
	key = strndup(_key, 16);
}

Device::~Device() {
	free(mac);
	free(key);
}