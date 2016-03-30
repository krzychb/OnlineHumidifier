/*
  Control of Humidifier Connected to a RC Socket
  https://github.com/krzychb/OnlineHumidifier
  Part A3 - http

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
// Measure ambient humidity using DHT sensor
//
void measureHumidity(void)
{
  humidity = dht.readHumidity();
  if (isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("MEASURE > ");
  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println("%");
}


