#include <ESP8266WiFi.h>
#include <GreeController.h>

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

    Serial.println();
    Serial.println("GreeController starting up..");
    greeController.listen();
    greeController.scan();
}

bool isNumeric(String in) {
  for(char c: in) {
    if(! isDigit(c))
      return false;
  }
  return true;
}

String a;
void loop() {
  while(Serial.available()) {
    a = Serial.readStringUntil('\n');
    int idx = a.indexOf(" ");

    if(idx != -1) { // multiple words
      String _first = a.substring(0, idx);
      String _second = a.substring(idx + 1);

      if(isNumeric(_second)) { // second word is a number
        int _val = _second.toInt();
        greeController.set(_first.c_str(), (uint8_t)_val, GREE_MAC);
      } else {
        greeController.set(_first.c_str(), _second.c_str(), GREE_MAC);
      }
    } else {
      greeController.get(a.c_str(), GREE_MAC);
    }
  }
}
