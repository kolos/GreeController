# ESP8266 Gree Aircooler Wifi Controller

An Arduino library for controlling your Gree (and clones) air cooler from the ESP8266

## Example

```cpp
#include <GreeController.h>

GreeController greeController;

void greeMsgHandler(const char* msg) {
  Serial.println(msg); // prints message, eg.: {"t":"dat","mac":"1337deadbeef","r":200,"cols":["Pow","Mod","SetTem","TemSen"],"dat":[1,1,25,26]}
}

void requestAcStatus() {
  for(auto device: greeController.getDevices()) {
    greeController.get("Pow\",\"Mod\",\"SetTem\",\"TemSen", device->mac); // for more status parameters please follow the link for gree protocol
  }
}

void setup() {
  /* .. set up wifi .. */
  greeController.setStatusHandler(greeMsgHandler); // set up message handler
  greeController.listen(); // sets up listener for responses
  greeController.scan(); // scans for AC devices on the network
}

void loop() {
  requestAcStatus(); // sends status request to the devices  

  delay(5 * 60 * 1000);
}
```

## Depends on external libs:
* https://github.com/me-no-dev/ESPAsyncUDP
* https://github.com/kokke/tiny-AES-c

## Protocol by:
* https://github.com/tomikaa87/gree-remote
