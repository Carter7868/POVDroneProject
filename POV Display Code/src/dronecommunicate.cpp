#include <Arduino.h>
#include <WiFi.h>
/*
* ESP32 drone communication script
* This script provides the functions to send and recive rpm data
*
* Run wifisetup() once at the start of the program
* Use readRPM() to read the current RPM
* Use setRPM(int rpm) to set the RPM
*
* Written by Carter Jardine
*/

//Wifi login credentials
const char* ssid = "POVDroneDisplay";
const char* password = "123456789";

void wifisetup() {
  //Initializes wifi connection (LED2 turns on when wifi is connected)
  digitalWrite(2, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
  digitalWrite(2, HIGH);
}
int readRPM() {
  //Reads the first HTML comment from the host server and returns it as an integer
  WiFiClient client;
  if (!client.connect("192.168.4.1", 80)) {
    Serial.println("Connection failed");
    return -1;
  }
  client.print(String("GET / HTTP/1.1\r\n") +
               "Host: " + "192.168.4.1" + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  String firstLine = client.readStringUntil('>');
  size_t pos = firstLine.indexOf("<!--");
  firstLine.remove(0,pos+4); //Remove Start of comment
  pos = firstLine.indexOf("--");
  firstLine.remove(pos); //Remove end of comment
  client.stop();
  return firstLine.toInt();
}
bool setRPM(int rpm) {
  //Sets the RPM by connecting to the url/set/rpm/value
  WiFiClient client;
  if (!client.connect("192.168.4.1", 80)) {
    Serial.println("Connection failed");
    client.stop();
    return false;
  }
  client.print(String("GET /set/rpm/") + rpm + " HTTP/1.1\r\n" +
               "Host: " + "192.168.4.1" + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
  client.stop();
  return true;
}
