// This is example file from https://github.com/esp8266/Arduino
// It has been modified for the purpose of OnlineHumidifier project - https://github.com/krzychb/OnlineHumidifier


//
// DHT Sensor Setup
//
#include "DHT.h"

//  dht(DHTPIN, DHTTYPE);
DHT dht(D2, DHT22);
float humidity;

//
// Wi-Fi Setup
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "********";     // your network SSID (name)
const char* password = "********";  // your network password


//
// Emoncms configuration
//
// Domain name of emoncms server - "emoncms.org"
// If unable to connect with domant name, use IP adress instead - "80.243.190.58"
const char* emoncmsServer = "emoncms.org";
//                       enter your Read & Write API Key below 
String apiKeyEmoncms = "9a3e3c9cf65c70a597097b065dcb24e3";
WiFiClient client;


void setup(void)
{
  dht.begin();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println();

  Serial.println("OnlineHumidifier-Chart.ino");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop(void)
{
  measureHumidity();
  sendDataToEmoncms();

  delay(20000);  // wait 20 seconds
}

