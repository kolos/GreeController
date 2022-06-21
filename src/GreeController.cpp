#include "GreeController.h"

using namespace GreeControllerLib;

void GreeController::handleStatusPacket(AsyncUDPPacket* packet) {
	const char* data = (char*)packet->data();

	const char* cid = getJsonValueStart(data, "cid");
	cid += getJsonValueIsString(cid);
	Device* device = findDeviceByMac(cid);
	if(device == nullptr) return;

	const char* packedJsonValueStart = getJsonValueStart(data, "pack");
	uint16_t jsonValueLength = getJsonValueLength(packedJsonValueStart);
	packedJsonValueStart += getJsonValueIsString(packedJsonValueStart);

	char* status = GreePacker::unpack(device->key, packedJsonValueStart, jsonValueLength);

	if(cb != nullptr) {
		cb(status);
	}

	free(status);
}

void GreeController::handleHandshakePacket(AsyncUDPPacket* packet) {
	const char* data = (char*)packet->data();
	const char* packedJsonValueStart = getJsonValueStart(data, "pack");
	uint16_t jsonValueLength = getJsonValueLength(packedJsonValueStart);
	packedJsonValueStart += getJsonValueIsString(packedJsonValueStart);

	char* unpacked = GreePacker::unpack(base_key, packedJsonValueStart, jsonValueLength);

	if(strstr_P(unpacked, PSTR("\"t\":\"dev\""))) {
		free(unpacked);
		sendBindingRequest(packet);
		return;
	}

	if(strstr_P(unpacked, PSTR("\"t\":\"bindok\""))) {
		const char* keyStart = getJsonValueStart(unpacked, "key");
		keyStart += getJsonValueIsString(keyStart);

		addDevice(packet, keyStart);
	}
	free(unpacked);
}

void GreeController::packetHandler(AsyncUDPPacket* packet) {
	const char* data = (char*)packet->data();

	if(strstr_P(data, PSTR("\"t\":\"pack\""))) { // has packed data
		if(strstr_P(data, PSTR("\"i\":1"))) { // initial packet
			handleHandshakePacket(packet);
		} else if(strstr_P(data, PSTR("\"i\":0"))) { // other packet
			handleStatusPacket(packet);
		}
	}
}

void GreeController::addDevice(AsyncUDPPacket* packet, const char* key) {
	const char* data = (char*)packet->data();

	const char* cid = getJsonValueStart(data, "cid");
	cid += getJsonValueIsString(cid);

	devices.push_back(new Device(cid, key, packet->remoteIP()));
}

void GreeController::sendBindingRequest(AsyncUDPPacket* packet) {
	const char* data = (char*)packet->data();

	const char* cid = getJsonValueStart(data, "cid");
	cid += getJsonValueIsString(cid);

	char binding_json[strlen_P(BINDING_STR) - 5 /* for %.12s */ + 12 /* mac addr */ + 1 /* for 0 terminal */];
	snprintf_P(binding_json, sizeof(binding_json),
		BINDING_STR,
		cid
	);

	char* packed = GreePacker::pack(base_key, binding_json);

	char bind_request[strlen_P(BINDING_REQUEST_STR) - 2 - 5 + 12 /* mac addr */ + strlen(packed) + 1];
	snprintf_P(bind_request, sizeof(bind_request),
		BINDING_REQUEST_STR,
		packed,
		cid
	);
	free(packed);

	udp.writeTo((uint8_t*)bind_request, sizeof(bind_request), packet->remoteIP(), GREE_PORT);
}

void GreeController::listen() {
	if(!udp.listen(GREE_PORT)) return;

	udp.onPacket([&](AsyncUDPPacket packet){
		packetHandler(&packet);
	});
}

void GreeController::scan() {
	char scanning[13];
	sprintf_P(scanning, SCANNING_STR);
	udp.broadcastTo(scanning, GREE_PORT);
}

Device* GreeController::findDeviceByMac(const char* cid) {
	for(Device* device: devices) {
		if(strncmp(cid, device->mac, 12 /* mac addr len */) == 0) {
			return device;
		}
	}

	return nullptr;
}

void GreeController::get(const char* input, const char* mac) {
	Device* device = findDeviceByMac(mac);
	if(device == nullptr) return;

	char status_json[strlen_P(ONE_STATUS_STR) - 2 -2 + strlen(input) + 12 /* mac addr */ + 1];
	snprintf_P(status_json, sizeof(status_json),
		ONE_STATUS_STR,
		input,
		device->mac
	);

	char* packed = GreePacker::pack(device->key, status_json);

	char status_request[strlen_P(STATUS_REQUEST_STR) - 2 + 12 /* mac addr */ + strlen(packed) + 1];
	snprintf_P(status_request, sizeof(status_request),
		STATUS_REQUEST_STR,
		packed,
		device->mac
	);
	free(packed);
	
	udp.writeTo((uint8_t*)status_request, strlen(status_request), device->ip, GREE_PORT);
}

uint8_t GreeController::numOfDecimals(uint8_t num){
	return num < 10 ? 1
					: num < 100 ? 2
					: 3;
}

void GreeController::set(const char* option, uint8_t value, const char* mac) {
	Device* device = findDeviceByMac(mac);
	if(device == nullptr) return;

	char status_json[strlen_P(SET_OPTION_STR) - 2 -2 + strlen(option) + numOfDecimals(value) + 12 /* mac addr */ + 1];
	snprintf_P(status_json, sizeof(status_json),
		SET_OPTION_STR,
		option,
		value,
		device->mac
	);

	char* packed = GreePacker::pack(device->key, status_json);

	char status_request[strlen_P(STATUS_REQUEST_STR) - 2 + 12 /* mac addr */ + strlen(packed) + 1];
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

	char status_json[strlen_P(SET_OPTIONS_STR) - 2 -2 + strlen(options) + strlen(values) + 12 /* mac addr */ + 1];
	snprintf_P(status_json, sizeof(status_json),
		SET_OPTIONS_STR,
		options,
		values,
		device->mac
	);

	char* packed = GreePacker::pack(device->key, status_json);

	char status_request[strlen_P(STATUS_REQUEST_STR) - 2 + 12 /* mac addr */ + strlen(packed) + 1];
	snprintf_P(status_request, sizeof(status_request),
		STATUS_REQUEST_STR,
		packed,
		device->mac
	);
	free(packed);

	udp.writeTo((uint8_t*)status_request, sizeof(status_request), device->ip, GREE_PORT);
}

void GreeController::setStatusHandler(GreeMsgHandlerFunction callback) {
	cb = callback;
}

void GreeController::rescan() {
	for(uint8_t i = 0; i<devices.size(); i++) {
		delete devices[i];
	}
	devices.clear();
	scan();
}

std::vector<Device*> GreeController::getDevices() {
	return devices;
}

const char* GreeController::getJsonValueStart(const char* json, const char* tag) {
	const char *search_idx = json;
	uint8_t tag_len = strlen(tag);
	while((search_idx = strstr(search_idx, tag))) {

		if(*(search_idx - 1) == '"' && *(search_idx + tag_len) == '"' && *(search_idx + tag_len + 1) == ':') {
			search_idx += tag_len+2;

			while(*(search_idx) == ' ' || *(search_idx) == '\t' || *(search_idx) == '\n') {
				search_idx++;
			}

			char first_char = *(search_idx);
			if(first_char == '"' || first_char == '0' || first_char == '1' || first_char == '2' || first_char == '3' || first_char == '4' || first_char == '5' || first_char == '6' || first_char == '7' || first_char == '8' || first_char == '9') break;

			if(strcasestr(search_idx, "null") == search_idx) break;

			return NULL;
		}

		search_idx+=tag_len+2;
	};

	return search_idx;
}

uint16_t GreeController::getJsonValueLength(const char* json) {
	if(json == NULL) return 0;
	if(strcasestr(json, "null") == json) return 4;
	char first_char = *(json);

	if(first_char == '"') json++;
	uint16_t val_len = 0;
	for(;*(json+val_len) != 0;val_len++) {
		char c = *(json+val_len);
		if(first_char == '"') {
			if(c == '"' && *(json+val_len-1) != '\\') break;
		} else {
			if(c == ',' || c == '}') break;
		}
	}

	return val_len;
}

bool GreeController::getJsonValueIsString(const char* json) {
	if(json == NULL) return 0;
	char first_char = *(json);

	return (first_char == '"');
}
