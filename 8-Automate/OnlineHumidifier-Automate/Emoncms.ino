/*
 Control of Humidifier Connected to a RC Socket
 https://github.com/krzychb/OnlineHumidifier
 Part 8 - Automate
 
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
// Send data to Emoncms
//
// set up of fields sent:
// Humidity = Ambient Humidity measured by DHT
// HumiditySP = Humidity setpoint
// Humidifier = status of humidifier - on / off
// AutoMode = status of automatic control - on = auto / off = manual
//
void sendDataToEmoncms(void)
{
  if (client.connect(emoncmsServer, 80))
  {
    String getJSON = "";
    getJSON += "GET http://" + (String) emoncmsServer;
    getJSON += "/input/post.json?json={";
    getJSON += "Humidity:" + (String) humidity + ",";
    getJSON += "HumiditySP:" + (String) humiditySetPoint + ",";
    getJSON += "Humidifier:" + (String) humidifier + ",";
    getJSON += "AutoMode:" + (String) autoMode;
    getJSON += "}&apikey=" + apiKeyEmoncms + "\n";
    getJSON += "Connection: close\n\n";
    client.print(getJSON);
    // display response from Emoncms
    while (client.available())
    {
      Serial.write(client.read());
    }
    Serial.println("Data sent to Emoncms");
  }
  else
  {
    Serial.println("Connection to Emoncms failed");
  }
  client.stop();
}

