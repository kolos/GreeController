#ifndef GREECONTROLLER_H
#define GREECONTROLLER_H

#include <Arduino.h>
#include <ESPAsyncUDP.h>
#include <ArduinoJson.h>
#include <vector>
#include <pgmspace.h>

#include "GreeDevice.h"
#include "GreePacker.h"

#include "GreeTemplates.h"

#define GREE_PORT 7000
#define BASE_KEY "a3K8Bx%2r8Y7#xDh"

namespace GreeControllerLib {

class GreeController
{
	private:
		AsyncUDP udp;
		std::vector<Device*> devices;
		const char* base_key = BASE_KEY;
		void handleStatusPacket(const JsonObject& root);
		void handleHandshakePacket(IPAddress remoteIP, const JsonObject& root);
		void packetHandler(AsyncUDPPacket packet);
		void addDevice(const char* mac, const char* key, IPAddress remoteIP);
		void sendBindingRequest(IPAddress remoteIP, const char* mac);
		Device* findDeviceByMac(const char* mac);
		uint8_t numOfDecimals(uint8_t num);
	public:
		void listen();
		void scan();
		void getStatus(const char* mac);
		void getThis(const char* input, const char* mac);
		void setThis(const char* option, uint8_t value, const char* mac);
		void setThis(const char* option, const char* values, const char* mac);
};

} // namespace GreeControllerLib

typedef GreeControllerLib::GreeController GreeController;

#endif