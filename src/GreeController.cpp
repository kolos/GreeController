#include "GreeController.h"

using namespace GreeControllerLib;

void GreeController::handleStatusPacket(const JsonObject& root) {
	Device* device = findDeviceByMac(root["cid"].as<char*>());
	if(device == nullptr) return;

	char* unpacked = GreePacker::unpack(device->key, root["pack"].as<char*>());

	DynamicJsonBuffer jsonBuffer(strlen(unpacked));
	JsonObject& unpacked_json = jsonBuffer.parseObject(unpacked);
	if(!unpacked_json.success()) return;

	unpacked_json.printTo(Serial);
	
	Serial.println("");

	free(unpacked);
}

void GreeController::handleHandshakePacket(IPAddress remoteIP, const JsonObject& root) {
	char* unpacked = GreePacker::unpack(base_key, root["pack"].as<char*>());

	DynamicJsonBuffer jsonBuffer(strlen(unpacked));
	JsonObject& unpacked_json = jsonBuffer.parseObject(unpacked);
	if(!unpacked_json.success()) return;

	const char* topic = unpacked_json["t"];
	if(strcmp_P(topic, PSTR("dev")) == 0) {
		sendBindingRequest(remoteIP, unpacked_json["mac"].as<char*>());
	}
	else if(strcmp_P(topic, PSTR("bindok")) == 0) {
		addDevice(unpacked_json["mac"].as<char*>(), unpacked_json["key"].as<char*>(), remoteIP);
	}

	free(unpacked);
}

void GreeController::packetHandler(AsyncUDPPacket packet) {
	if(packet.remotePort() != GREE_PORT) return;

	DynamicJsonBuffer jsonBuffer(packet.length());
	JsonObject& root = jsonBuffer.parseObject(packet.data());
	if(!root.success()) return;

	const char* topic = root["t"];
	if(topic == nullptr ) return;
	int i = root["i"];

	if(i == 1 && strcmp_P(topic, PSTR("pack")) == 0) {
		handleHandshakePacket(packet.remoteIP(), root);
	}else if(i == 0 && strcmp_P(topic, PSTR("pack")) == 0) {
		handleStatusPacket(root);
	}
}

void GreeController::addDevice(const char* mac, const char* key, IPAddress remoteIP) {
	devices.push_back(new Device(mac, key, remoteIP));
}

void GreeController::sendBindingRequest(IPAddress remoteIP, const char* mac) {

	char binding_json[strlen_P(BINDING_STR) - 2 /* for %s */ + strlen(mac) + 1 /* for 0 terminal */];
		snprintf_P(
		binding_json, sizeof(binding_json),
		BINDING_STR,
		mac
	);
  
	char* packed = GreePacker::pack(base_key, binding_json);

	char bind_request[strlen_P(BINDING_REQUEST_STR) - 2 - 2 + strlen(mac) + strlen(packed) + 1];
		snprintf_P(bind_request, sizeof(bind_request),
		BINDING_REQUEST_STR,
		packed,
		mac
	);

	udp.writeTo((uint8_t*)bind_request, sizeof(bind_request), remoteIP, GREE_PORT);

	free(packed);
}

void GreeController::listen() {
	if(udp.listen(0)) {
		udp.onPacket([this](AsyncUDPPacket packet){
			packetHandler(packet);
		});
	}
}

void GreeController::scan() {
	char scanning[13];
	sprintf_P(scanning, SCANNING_STR);
	udp.broadcastTo(scanning, GREE_PORT);
}

void GreeController::getStatus(const char* mac) {
	Device* device = findDeviceByMac(mac);
	if(device == nullptr) return;

	char status_json[strlen_P(STATUS_STR) - 2 + strlen(device->mac) + 1];
	snprintf_P(
		status_json, sizeof(status_json),
		STATUS_STR,
		device->mac
	);

	char* packed = GreePacker::pack(device->key, status_json);

	char status_request[strlen_P(STATUS_REQUEST_STR) - 2 + strlen(device->mac) + strlen(packed) + 1];
	snprintf_P(status_request, sizeof(status_request),
		STATUS_REQUEST_STR,
		packed,
		device->mac
	);

	udp.writeTo((uint8_t*)status_request, sizeof(status_request), device->ip, GREE_PORT);

	free(packed);
}

Device* GreeController::findDeviceByMac(const char* mac) {
	for(Device* device: devices) {
		if(strncmp(mac, device->mac, strlen(mac)) == 0) {
			return device;
		}
	}

	return nullptr;
}

void GreeController::getThis(const char* input, const char* mac) {
	Device* device = findDeviceByMac(mac);
	if(device == nullptr) return;
	
	char status_json[strlen_P(ONE_STATUS_STR) - 2 -2 + strlen(input) + strlen(device->mac) + 1];
	snprintf_P(
		status_json, sizeof(status_json),
		ONE_STATUS_STR,
		input,
		device->mac
	);

	char* packed = GreePacker::pack(device->key, status_json);

	char status_request[strlen_P(STATUS_REQUEST_STR) - 2 + strlen(device->mac) + strlen(packed) + 1];
	snprintf_P(status_request, sizeof(status_request),
		STATUS_REQUEST_STR,
		packed,
		device->mac
	);

	udp.writeTo((uint8_t*)status_request, sizeof(status_request), device->ip, GREE_PORT);

	free(packed);
}

uint8_t GreeController::numOfDecimals(uint8_t num){
	return num < 10 ? 1
					: num < 100 ? 2
					: 3;
}

void GreeController::setThis(const char* option, uint8_t value, const char* mac) {
	Device* device = findDeviceByMac(mac);
	if(device == nullptr) return;

	char status_json[strlen_P(SET_OPTION_STR) - 2 -2 + strlen(option) + numOfDecimals(value) + strlen(device->mac) + 1];
	snprintf_P(
		status_json, sizeof(status_json),
		SET_OPTION_STR,
		option,
		value,
		device->mac
	);

	char* packed = GreePacker::pack(device->key, status_json);

	char status_request[strlen_P(STATUS_REQUEST_STR) - 2 + strlen(device->mac) + strlen(packed) + 1];
	snprintf_P(status_request, sizeof(status_request),
		STATUS_REQUEST_STR,
		packed,
		device->mac
	);

	udp.writeTo((uint8_t*)status_request, sizeof(status_request), device->ip, GREE_PORT);

	free(packed);
}

void GreeController::setThis(const char* options, const char* values, const char* mac) {
	Device* device = findDeviceByMac(mac);
	if(device == nullptr) return;

	char status_json[strlen_P(SET_OPTIONS_STR) - 2 -2 + strlen(options) + strlen(values) + strlen(device->mac) + 1];
	snprintf_P(
		status_json, sizeof(status_json),
		SET_OPTIONS_STR,
		options,
		values,
		device->mac
	);

	char* packed = GreePacker::pack(device->key, status_json);

	char status_request[strlen_P(STATUS_REQUEST_STR) - 2 + strlen(device->mac) + strlen(packed) + 1];
	snprintf_P(status_request, sizeof(status_request),
		STATUS_REQUEST_STR,
		packed,
		device->mac
	);

	udp.writeTo((uint8_t*)status_request, sizeof(status_request), device->ip, GREE_PORT);

	free(packed);
}
