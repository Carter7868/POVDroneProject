#include <Arduino.h>
#include <WiFi.h>
#include "dronecommunicate.h"

void setup() {
  Serial.begin(115200);
  wifisetup();
}

void loop() {
  //Serial.println(readRPM());
  //Delay for 5 seconds then set rpm to a random number between 0 and 1000
  delay(5000);
  setRPM(random(1000));
}