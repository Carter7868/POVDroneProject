#include <Arduino.h>
#include <WiFi.h>
/*
* ESP32 Drone Communication Script
* Creates a soft access point with UI controls
* 
* Written by Carter Jardine
*/

//ESP32 Network Soft Access Point
const char* ssid = "POVDroneDisplay";
const char* password = "123456789";
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Auxiliar variable to store the current motor output state and rmp target
String motorOutput = "off";
int rpmTarget = 0;
#define LED 2


void setup() {
  Serial.begin(115200);
  //Set LED pin as output and turn it off
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Create soft access point
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
}


void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("----------New Client-----------");
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Check for GET request and change variables accordinly
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              motorOutput = "on";
              digitalWrite(LED, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              motorOutput = "off";
              digitalWrite(LED, LOW);
            } else if (header.indexOf("GET /set/rpm/") >=0) {
              rpmTarget = header.substring(13, 17).toInt();
              Serial.println("RPM Set to: ");
              Serial.println(rpmTarget);
              Serial.println("RPM");
            }

            // Output a 4 character random number to the HTML page as a comment (Convert to RPM in future)
            String output;
            String rpm = String(random(1000));
            while (rpm.length() != 4 && rpm.length() < 4) {
              rpm = rpm + " ";
            }
            output = rpm;
            client.println("<!--" + output + "-->");

            // Display the HTML web page (This part was really fun /s)
            //For copy paste: client.println("");
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<style type=\"text/css\">.highlight {color: #BB86FC} #setrpm{height:50px; width: 100px; font-size:30pt;} #button{height:50px; font-size:30pt;} </style>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<body style=\"background-color:#121212; color:#FFFFFF\">");
            client.println("<h1 style=\"font-size:40px\"><center>POV Drone Display Controller</center></h1>");
            client.println("<p1 style=\"font-size:30px\"><center>Motor State: <span class=\"highlight\"> " + motorOutput + " </span></center></p1>");
            client.println("<p1 style=\"font-size:30px\"><center>Spin Speed: <span class=\"highlight\"> " + String(rpmTarget) + " </span>rpm</center></p1>");
            client.println("<p1 style=\"font-size:30px\"><center>Altitude: <span class=\"highlight\">0</span> m</center></p1><br><br>");
            client.println("<form action=\"/action_page.php\" style=\"font-size:50px\"><center>");
            client.println("<input type=\"number\" id=\"setrpm\" name=\"setrpm\" value=\"400\"><br>");
            client.println("<input type=\"submit\" id=\"button\" value=\"Set RPM\">");
            client.println("</center></form>");
            client.println("<p><center><a href=\"/26/on\"><button class=\"button\" id=\"button\">Enable Motor</button></a></center></p>");
            client.println("<p><center><a href=\"/26/off\"><button class=\"button\" id=\"button\">Disable Motor</button></a></center></p>");
            client.println("</body>");
            client.println("</html>");
            client.println("");
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("----------Client disconnected----------");
    Serial.println("");
  }
}