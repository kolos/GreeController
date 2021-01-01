#include "GreeDevice.h"

using namespace GreeControllerLib;

Device::Device(const char* _mac, const char* _key, IPAddress _ip): ip(_ip), last_input(NULL), last_query(NULL), last_status(NULL), last_packed_status(NULL) {
	mac = strndup(_mac, 12);
	key = strndup(_key, 16);
}

Device::~Device() {
	free(mac);
	free(key);
	free(last_input);
	free(last_query);
	free(last_status);
	free(last_packed_status);
}