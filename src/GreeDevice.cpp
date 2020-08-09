#include "GreeDevice.h"

using namespace GreeControllerLib;

Device::Device(const char* _mac, const char* _key, IPAddress _ip): ip(_ip), last_input(NULL), last_query(NULL) {
	strncpy(mac, _mac, 12);
	strncpy(key, _key, 16);
	mac[12] = 0; key[16] = 0;

	Serial.print("New device: ");
	Serial.print("mac=");
	Serial.print(mac);
	Serial.print(" key=");
	Serial.print(key);
	Serial.print(" ip=");
	Serial.print(ip);
	Serial.println("");
}

Device::~Device() {
	if(last_input)
		free(last_input);

	if(last_query)
		free(last_query);
}