#include <ESP8266WiFi.h>
#include "src/gree/controller.h"

#include "constants.h"

GreeController greeController;
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println(F("WiFi Failed"));
        while(1) {
            delay(1000);
        }
    }

    greeController.listen();
    greeController.scan();
}

String a;
void loop() {
  while(Serial.available()) {
    a = Serial.readStringUntil('\n');

    const char* thing = a.c_str();
    greeController.getThis(thing, GREE_MAC);
  }
  /*
  delay(5000);
  greeController.getStatus(GREE_MAC);
  delay(60 * 5000);
  */
}
