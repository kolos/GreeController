#include "devices.h"

void Devices::handleHandshakePacket(IPAddress remoteIP, const JsonObject& root) {
  char* unpacked = GreePacker::unpack(base_key, root["pack"].as<char*>());

  DynamicJsonBuffer jsonBuffer(strlen(unpacked));
  JsonObject& unpacked_json = jsonBuffer.parseObject(unpacked);
  if(!unpacked_json.success()) return;

  const char* topic = unpacked_json["t"];
  if(strcmp(topic, "dev") == 0) {
	sendBindingRequest(remoteIP, unpacked_json["mac"].as<char*>());
  }
  else if(strcmp(topic, "bindok") == 0) {
	addDevice(unpacked_json["mac"].as<char*>(), unpacked_json["key"].as<char*>(), remoteIP);
  }
 
  free(unpacked);
}

void Devices::packetHandler(AsyncUDPPacket packet) {
  if(packet.remotePort() != GREE_PORT) return;

  DynamicJsonBuffer jsonBuffer(packet.length());
  JsonObject& root = jsonBuffer.parseObject(packet.data());
  if(!root.success()) return;

  const char* topic = root["t"];
  if(topic == nullptr ) return;

  if(strcmp(topic, "pack") == 0) {
	handleHandshakePacket(packet.remoteIP(), root);
  }
}

void Devices::addDevice(const char* mac, const char* key, IPAddress remoteIP) {
  devices.push_back(Device(mac, key, remoteIP));
}

void Devices::sendBindingRequest(IPAddress remoteIP, const char* mac) {
  String binding_json = "{\"mac\":\"" + String(mac) + "\",\"t\":\"bind\",\"uid\": 0}";
  char* packed = GreePacker::pack(base_key, binding_json.c_str());
  String bind_request = "{\"cid\":\"app\",\"i\":1,\"pack\":\"" + String(packed) + "\",\"t\":\"pack\",\"tcid\":\"" + String(mac) + "\",\"uid\":0}";

  udp.broadcastTo((uint8_t*)bind_request.c_str(), remoteIP, GREE_PORT);
  
  free(packed);
}

void Devices::listen() {
  if(udp.listen(0)) {
	udp.onPacket([this](AsyncUDPPacket packet){
	  packetHandler(packet);
	});
  }
}

void Devices::scan() {
  udp.broadcastTo("{\"t\": \"scan\"}", GREE_PORT);
}