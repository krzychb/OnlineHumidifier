/*
  Control of Humidifier Connected to a RC Socket
  https://github.com/krzychb/OnlineHumidifier
  Part A1 - MQTT

  Copyright (c) 2015 Krzysztof Budzynski. All rights reserved.
  This file is part of OnlineHumidifier project - https://github.com/krzychb/OnlineHumidifier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/



//
// DHT Sensor Setup
//
#include "DHT.h"
//  dht(DHTPIN, DHTTYPE);
DHT dht(D2, DHT22);
float humidity;
float humiditySetPoint = 43;
// function prototype required by Arduino IDE 1.6.7
void measureHumidity(void);


//
// RF 433 transmitter configuration
//
// download - https://bitbucket.org/fuzzillogic/433mhzforarduino/wiki/Home/
//
#include <RemoteTransmitter.h>
#define RF433_TR_PIN D1   // pin where RF433 Transmitter is connected to
ActionTransmitter actionTransmitter(RF433_TR_PIN);
bool humidifier = LOW;
bool autoMode = false;
char rcDevice = 'A';
// function prototype required by Arduino IDE 1.6.7
void executeHumidityControl(void);


//
// Wi-Fi Setup
//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
const char* ssid = "********";     // your network SSID (name)
const char* password = "********";  // your network password
ESP8266WebServer server(80);
// function prototypes required by Arduino IDE 1.6.7
void setupWiFi(void);
void handleRoot(void);
void handleNotFound(void);
void showControlScreen(void);
void setupWebserver(void);


//
// OTA configuration
//
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
const char* host = "OTA-Humidifier";
// function prototypes required by Arduino IDE 1.6.7
void setupOTA(void);


//
// Emoncms configuration
//
// Domain name of emoncms server - "emoncms.org"
// If unable to connect with domant name, use IP adress instead - "80.243.190.58"
const char* emoncmsServer = "emoncms.org";
//                       enter your Read & Write API Key below 
String apiKeyEmoncms = "be71f01adf17bfa1a85118923c0140b4";
WiFiClient client;
// function prototype required by Arduino IDE 1.6.7
void sendDataToEmoncms(void);


//
// MQTT configuration
//
#include <PubSubClient.h>
WiFiClient wclient;
PubSubClient MQTTclient(wclient, "test.mosquitto.org");


void setup(void)
{
  Serial.begin(115200);

  dht.begin();
  setupWiFi();
  setupWebserver();
  setupOTA();
}


void loop(void)
{
  ArduinoOTA.handle();
  server.handleClient();

  // execute actions below
  // evey 20 seconds
  if (millis() % 20000 == 0)
  {
    measureHumidity();
    if (autoMode == true)
    {
      executeHumidityControl();
    }

    sendDataToEmoncms();

    if (!MQTTclient.connected())
    {
      // connect if not already connected
      MQTTclient.connect("arduinoClient");
    }
    // check if we are indeed connected and publish
    if (MQTTclient.connected())
    {
      MQTTclient.publish("krzychb/home/sensor/Humidity", (String) humidity);
      MQTTclient.publish("krzychb/home/hygrostat/HumiditySetPoint", (String) humiditySetPoint);
      MQTTclient.publish("krzychb/home/hygrostat/Humidifier", (humidifier == HIGH) ? "ON" : "OFF");
      MQTTclient.publish("krzychb/home/hygrostat/AutoMode", (autoMode == true) ? "ON" : "OFF");
    }
    else
    {
      Serial.println("connection failed!");
    }
  }
}

