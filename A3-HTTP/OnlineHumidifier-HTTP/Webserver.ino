/*
  Control of Humidifier Connected to a RC Socket
  https://github.com/krzychb/OnlineHumidifier
  Part B1 - http

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


void setupWiFi(void)
{
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
}



void handleRoot(void)
{
  String message = "hello from esp8266!";
  message += "\nHumidity = ";
  message += (String) humidity;
  message += "%";

  server.send(200, "text/plain", message);
}


void handleNotFound(void)
{
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


//
// web server set up and events
//
void setupWebserver(void)
{

  server.on("/", handleRoot);

  server.on("/hygrostat", []() {
    Serial.print("HTTP REQUEST > ");

    for (uint8_t i = 0; i < server.args(); i++)
    {
      if (server.argName(i) == "Humidifier")
      {
        humidifier = (server.arg(i) == "ON") ? HIGH : LOW;
        actionTransmitter.sendSignal(1, rcDevice, humidifier);
      }
      else if (server.argName(i) == "AutoMode")
      {
        autoMode = (server.arg(i) == "ON") ? true : false;
      }
      else if (server.argName(i) == "HumiditySetPoint")
      {
        humiditySetPoint = server.arg(i).toFloat();
      }
      else
      {
        Serial.println("unknown argument! ");
      }
      Serial.print(server.argName(i));
      Serial.print(": ");
      Serial.print(server.arg(i));
      Serial.print(" > ");
    }
    Serial.println("done");

    showControlScreen();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}


void showControlScreen(void)
{
  String message;
  message += "<html>";
  message += "<head><meta http-equiv=\"refresh\" content=\"5; url='/hygrostat\"'><title>Online Humidifier</title></head>";
  message += "<body>";
  message += "<h3>Hygrostat</h3>";
  message += "Humidity: " + (String) humidity + "%<br/>";
  message += "Setpoint: " + (String) humiditySetPoint + "%";
  message += "<form action=\"/hygrostat\" method=\"get\">";
  message += "Change <input type=\"text\" name=\"HumiditySetPoint\" size=\"3\" value=\"" + (String) humiditySetPoint + "\"><input type=\"submit\" value=\"Submit\">";
  message += "</form>";
  message += "Humidifier: ";
  if (humidifier == true)
  {
    message += "ON, switch it <a href=\"/hygrostat?Humidifier=OFF\">OFF</a><br/>";
  }
  else
  {
    message += "OFF, switch it <a href=\"/hygrostat?Humidifier=ON\">ON</a><br/>";
  }
  message += "Auto Mode: ";
  if (autoMode == true)
  {
    message += "ON, turn it <a href=\"/hygrostat?AutoMode=OFF\">OFF</a><br/>";
  }
  else
  {
    message += "OFF, turn it <a href=\"/hygrostat?AutoMode=ON\">ON</a><br/>";
  }
  message += "</body>";
  message += "</html>";
  server.send(200, "text/html", message);
}


//
// OTA set up and events
//
void setupOTA(void)
{
  ArduinoOTA.setHostname(host);

  ArduinoOTA.onStart([]() {
    Serial.println("OTA upload start");
    // switch humidifier off in case OTA fails
    actionTransmitter.sendSignal(1, rcDevice, LOW);
    Serial.println("Humidifier switched off");

  });

  ArduinoOTA.onEnd([]() {
    Serial.println("OTA upload end");
    Serial.println("Restarting...");
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

