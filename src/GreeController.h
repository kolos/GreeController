#ifndef GREECONTROLLER_H
#define GREECONTROLLER_H

#include <Arduino.h>
#include <ESPAsyncUDP.h>
#include <vector>
#include <pgmspace.h>

#include "GreeDevice.h"
#include "GreePacker.h"

#include "GreeTemplates.h"

#define GREE_PORT 7000
#define BASE_KEY "a3K8Bx%2r8Y7#xDh"

namespace GreeControllerLib {

typedef std::function<void(const char*)> GreeMsgHandlerFunction;

class GreeController
{
	private:
		AsyncUDP udp;
		std::vector<Device*> devices;
		GreeMsgHandlerFunction cb = nullptr;
		const char* base_key = BASE_KEY;
		void handleStatusPacket(AsyncUDPPacket packet);
		void handleHandshakePacket(AsyncUDPPacket packet);
		void packetHandler(AsyncUDPPacket packet);
		void addDevice(const char* mac, const char* key, IPAddress remoteIP);
		void sendBindingRequest(IPAddress remoteIP, const char* mac);
		Device* findDeviceByMac(const char* mac);
		uint8_t numOfDecimals(uint8_t num);
		char* getJsonValue(const char* json, const char* tag);
	public:
		void listen();
		void scan();
		void get(const char* input, const char* mac);
		void set(const char* option, uint8_t value, const char* mac);
		void set(const char* option, const char* values, const char* mac);
		void setStatusHandler(GreeMsgHandlerFunction cb);
};

} // namespace GreeControllerLib

typedef GreeControllerLib::GreeController GreeController;

#endif