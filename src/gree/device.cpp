#include "device.h"

Device::Device(const char* _mac, const char* _key, IPAddress _ip): ip(_ip) {
	mac = strdup(_mac);
	key = strdup(_key);

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
	free(mac);
	free(key);
}