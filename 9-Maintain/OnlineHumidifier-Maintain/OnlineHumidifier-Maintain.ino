// This is example file from https://github.com/esp8266/Arduino
// It has been modified for the purpose of OnlineHumidifier project - https://github.com/krzychb/OnlineHumidifier


//
// DHT Sensor Setup
//
#include "DHT.h"
//  dht(DHTPIN, DHTTYPE);
DHT dht(D2, DHT22);
float humidity;
float humiditySetPoint = 43;
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


//
// OTA configuration
//
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
const char* host = "OTA-Humidifier";


//
// emonCMS configuration
//
//                       enter your Write API Key below
String apiKeyEmoncms = "be71f01adf17bfa1a85118923c0140b4";
WiFiClient client;
void sendDataToEmoncms(void);


void handleRoot() {
  String message = "hello from esp8266!";
  message += "\nHumidity = ";
  message += (String) humidity;
  message += "%";

  server.send(200, "text/plain", message);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void showControlScreen(void) {
  String message;
  message += "<html>";
  message += "<head><meta http-equiv='refresh' content='20'/><title>Online Humidifier</title></head>";
  message += "<body>";
  message += "<h3>Humidifier Control - ";
  message += (String) (autoMode == true ? "Auto" : "Manual");
  message += "</h3>";
  message += "<p>";
  message += "Humidity : " + (String) humidity + "%<br />";
  message += "Humidifier : " + (String) humidifier + "<br />";
  message += "Operate : ";
  message += "<a href=\"/humidifier/1\">On</a>";
  message += " / ";
  message += "<a href=\"/humidifier/0\">Off</a>";
  message += " / ";
  message += "<a href=\"/humidifier/Auto\">Auto</a>";
  message += "</body>";
  message += "</html>";

  server.send(200, "text/html", message);
}


void setup(void) {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);
  Serial.println();

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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // web server set up and events
  server.on("/", handleRoot);

  server.on("/humidifier/1", []() {
    autoMode = false;
    humidifier = HIGH;
    actionTransmitter.sendSignal(1, rcDevice, humidifier);
    showControlScreen();
  });

  server.on("/humidifier/0", []() {
    autoMode = false;
    humidifier = LOW;
    actionTransmitter.sendSignal(1, rcDevice, humidifier);
    showControlScreen();
  });

  server.on("/humidifier/Auto", []() {
    autoMode = true;
    showControlScreen();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  // OTA set up and events
  ArduinoOTA.setHostname(host);
  
  ArduinoOTA.onStart([]() {
    Serial.println("OTA upload start");
    // switch humidifier off in case OTA fails
    actionTransmitter.sendSignal(1, rcDevice, LOW);
    Serial.println("Humidifier switched off");

  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA upload end");
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  Serial.println("OTA initialized");
}

void loop(void)
{
  ArduinoOTA.handle();
  server.handleClient();

  // measure, control and send to Emoncms.org
  // evey 20 seconds
  if (millis() % 20000 == 0)
  {
    measureHumidity();
    if (autoMode == true)
    {
      executeHumidityControl();
    }
    sendDataToEmoncms();
  }
}

