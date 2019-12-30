#include <ESP8266WiFi.h>
#include <GreeController.h>

#define SERIAL_BAUD_RATE 115200

#define WIFI_SSID		""
#define WIFI_PASSWORD		""

#define GREE_MAC		"1337deadbeef"

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
    greeController.get(thing, GREE_MAC);
  }
}
