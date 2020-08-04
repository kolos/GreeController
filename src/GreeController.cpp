#include "GreeController.h"

using namespace GreeControllerLib;

void GreeController::handleStatusPacket(AsyncUDPPacket packet) {
	char* data = (char*)packet.data();
	char* cid = getJsonValue(data, "cid");
	Device* device = findDeviceByMac(cid);
	if(device == nullptr) return;
	free(cid);

	char* packed = getJsonValue(data, "pack");
	char* unpacked = GreePacker::unpack(device->key, packed);
	free(packed);

	if(cb != nullptr) {
		cb(unpacked);
	}

	free(unpacked);
}

void GreeController::handleHandshakePacket(AsyncUDPPacket packet) {
	char* data = (char*)packet.data();
	char* packed = getJsonValue(data, "pack");

	char* unpacked = GreePacker::unpack(base_key, packed);
	free(packed);

	char* mac = getJsonValue(unpacked, "mac");
	if(strstr_P(unpacked, PSTR("\"t\":\"dev\""))) {
		free(unpacked);
		sendBindingRequest(packet.remoteIP(), mac);
		free(mac);
		return;
	}

	if(strstr_P(unpacked, PSTR("\"t\":\"bindok\""))) {
		char* key = getJsonValue(unpacked, "key");
		free(unpacked);
		addDevice(mac, key, packet.remoteIP());
		free(key);
		free(mac);
	}
}

void GreeController::packetHandler(AsyncUDPPacket packet) {
	if(packet.remotePort() != GREE_PORT) return;

	const char* data = (char*)packet.data();

	if(strstr_P(data, PSTR("\"t\":\"pack\""))) { // has packed data
		if(strstr_P(data, PSTR("\"i\":1"))) { // initial packet
			handleHandshakePacket(packet);
		} else if(strstr_P(data, PSTR("\"i\":0"))) { // other packet
			handleStatusPacket(packet);
		}
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
	free(packed);

	udp.writeTo((uint8_t*)bind_request, sizeof(bind_request), remoteIP, GREE_PORT);
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

Device* GreeController::findDeviceByMac(const char* mac) {
	for(Device* device: devices) {
		if(strncmp(mac, device->mac, strlen(mac)) == 0) {
			return device;
		}
	}

	return nullptr;
}

void GreeController::get(const char* input, const char* mac) {
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
	free(packed);

	udp.writeTo((uint8_t*)status_request, sizeof(status_request), device->ip, GREE_PORT);
}

uint8_t GreeController::numOfDecimals(uint8_t num){
	return num < 10 ? 1
					: num < 100 ? 2
					: 3;
}

void GreeController::set(const char* option, uint8_t value, const char* mac) {
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
	free(packed);

	udp.writeTo((uint8_t*)status_request, sizeof(status_request), device->ip, GREE_PORT);
}

void GreeController::set(const char* options, const char* values, const char* mac) {
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
	free(packed);

	udp.writeTo((uint8_t*)status_request, sizeof(status_request), device->ip, GREE_PORT);
}

char* GreeController::getJsonValue(const char* json, const char* tag) {
	char* search_tag = (char*)malloc(strlen(tag) + 3 + 1);
	snprintf(search_tag, strlen(tag) + 3 + 1, "\"%s\":", tag);

	char* search_idx = strstr(json, search_tag);
	free(search_tag);

	if(search_idx == nullptr) return nullptr;
	bool is_string = (*search_idx == '"') ? 1 : 0;
	search_idx += strlen(tag) + 3 + is_string * 1;
	uint16_t val_len = 0;
	for(;*search_idx!='\0';val_len++) {
		char c = *(search_idx+val_len);
		if(is_string) {
			if(c == '"' && *(search_idx+val_len-1) != '\\') break;
		} else {
			if(c == ',' || c == '}') break;
		}
	}

	char* tmp = (char*)malloc(val_len+1);
	strncpy(tmp, search_idx, val_len);
	tmp[val_len] = '\0';

	return tmp;
}

void GreeController::setStatusHandler(GreeMsgHandlerFunction callback) {
	cb = callback;
}

void GreeController::rescan() {
	devices.clear();
	scan();
}

std::vector<Device*> GreeController::getDevices() {
	return devices;
}