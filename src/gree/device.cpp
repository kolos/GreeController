#include "device.h"

Device::Device(const char* mac, const char* key, IPAddress ip):mac(mac), key(key), ip(ip) {
  Serial.print("New device: ");
  Serial.print("mac=");
  Serial.print(mac);
  Serial.print(" key=");
  Serial.print(key);
  Serial.println("");
}