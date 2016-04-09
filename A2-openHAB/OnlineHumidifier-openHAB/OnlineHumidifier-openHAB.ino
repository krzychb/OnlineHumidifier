/*
  Control of Humidifier Connected to a RC Socket
  https://github.com/krzychb/OnlineHumidifier
  Part A2 - openHAB

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
bool autoMode = true;
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
const char* host = "OTA-hygrostat";
// function prototypes required by Arduino IDE 1.6.7
void setupOTA(void);


//
// Emoncms configuration
//
// Domain name of emoncms server - "emoncms.org"
// If unable to connect with domant name, use IP adress instead - "80.243.190.58"
const char* emoncmsServer = "emoncms.org";
//                       enter your Read & Write API Key below 
String apiKeyEmoncms = "9a3e3c9cf65c70a597097b065dcb24e3";
WiFiClient client;
// function prototypes required by Arduino IDE 1.6.7
void sendDataToEmoncms(void);


//
// MQTT configuration
//
#include <PubSubClient.h>
WiFiClient wclient;
PubSubClient MQTTclient(wclient, "test.mosquitto.org");
const char* MQTTclientName = "hygrostat";

void MQTTcallback(const MQTT::Publish& pub)
{
  Serial.print("MQTT CALLBACK > ");
  Serial.print(pub.topic());
  Serial.print(" ");
  Serial.println(pub.payload_string());

  if (pub.topic() == "krzychb/openhab/hygrostat/HumiditySetPoint")
  {
    // adjust the stepoint
    humiditySetPoint = pub.payload_string().toFloat();
  }

  if (pub.topic() == "krzychb/openhab/hygrostat/Humidifier")
  {
    // turn humidifier on or off from openHAB
    humidifier = (pub.payload_string() == "ON") ? HIGH : LOW;
    actionTransmitter.sendSignal(1, rcDevice, humidifier);
  }

  // turn the control mode on or off from openHAB
  if (pub.topic() == "krzychb/openhab/hygrostat/AutoMode")
  {
    autoMode = (pub.payload_string() == "ON") ? true : false;
  }
}


void MQTTpublishData(void)
{
  Serial.print("MQTT PUBLISH / SUBSCRIBE > ");
  if (!MQTTclient.connected())
  {
    Serial.print("connecting > ");
    MQTTclient.connect(MQTTclientName);
  }
  if (MQTTclient.connected())
  {
    MQTTclient.publish("krzychb/home/sensor/Humidity", (String) humidity);
    MQTTclient.publish("krzychb/home/hygrostat/HumiditySetPoint", (String) humiditySetPoint);
    MQTTclient.publish("krzychb/home/hygrostat/Humidifier", (humidifier == HIGH) ? "ON" : "OFF");
    MQTTclient.publish("krzychb/home/hygrostat/AutoMode", (autoMode == true) ? "ON" : "OFF");
    MQTTclient.subscribe("krzychb/openhab/hygrostat/#");
    Serial.println("done");
  }
  else
  {
    Serial.println("connection failed!");
  }
}


void MQTTkeepAlive(void)
{
  // keep MQTT connected
  if (!MQTTclient.connected())
  {
    MQTTclient.connect(MQTTclientName);
  }
  else
  {
    // handle incoming MQTT messages
    MQTTclient.loop();
  }
}


void setup(void)
{
  Serial.begin(115200);

  dht.begin();
  setupWiFi();
  setupWebserver();
  setupOTA();

  MQTTclient.set_callback(MQTTcallback);
}


void loop(void)
{

  // monitor / keep alive connections
  ArduinoOTA.handle();
  server.handleClient();
  MQTTkeepAlive();

  // execute actions below evey 45 seconds
  if (millis() % 45000 == 0)
  {
    measureHumidity();
    if (autoMode == true)
    {
      executeHumidityControl();
    }
    sendDataToEmoncms();
    MQTTpublishData();
  }
}

