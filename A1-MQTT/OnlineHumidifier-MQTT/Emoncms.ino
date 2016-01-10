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
// Send data to Emoncms
//
// set up of fields sent:
// Humidity = Ambient Humidity measured by DHT
// HumiditySP = Humidity setpoint
// Humidifier = status of humidifier - on / off
//
void sendDataToEmoncms(void)
{
  if (client.connect("emoncms.org", 80))
  {
    String dataString = "";
    dataString += "Humidity:";
    dataString += (String) humidity;
    dataString += ",HumiditySP:";
    dataString += (String) humiditySetPoint;
    dataString += ",Humidifier:";
    dataString += (String) humidifier;
    dataString += ",AutoMode:";
    dataString += (String) autoMode;

    client.print("GET http://emoncms.org/input/post.json?json={");
    client.print(dataString);
    client.print("}&apikey=");
    client.println(apiKeyEmoncms);
    client.println("Connection: close");
    client.println();
    // display response from Emoncms
    while (client.available())
    {
      Serial.write(client.read());
    }
    Serial.println("Data sent to Emoncms");
  }
  else
  {
    Serial.println("Unable to connect to http://emoncms.org");
  }
  client.stop();
}

