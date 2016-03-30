/*
  Control of Humidifier Connected to a RC Socket
  https://github.com/krzychb/OnlineHumidifier
  Part A4 - Persistence

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
// Humidifyer Control
//
// Switch humidifyer On if measured humidity is below set point
// Switch it off if it is above the set point
// Add hysteresis of 2 % RH above and below the set point
// so RC socket and humidifyer are not abused by frequent switching
//
void executeHumidityControl(void)
{
  Serial.print("CONTROL > Humidity: ");
  Serial.print(humidity, 1);
  Serial.print("%, SetPoint: ");
  Serial.print(humiditySetPoint, 1);
  Serial.print("%, Humidifier: ");
  if (humidity > humiditySetPoint + 2)
  {
    // ambient humidity is above the set point
    // action : switch humidifyer Off
    humidifier = LOW;
  }
  else if (humidity < humiditySetPoint - 2)
  {
    // ambient humidity is below the set point
    // action : switch humidifyer On
    humidifier = HIGH;
  }
  else
  {
    // ambient humidity is within +/- 2% hysteresis
    // therefore do not alter curent status of humidifyer
    Serial.print("(No Change) ");
  }
  Serial.println(humidifier == HIGH ? "On" : "Off");

  //
  // Set first two parameters below to match your RC socket
  //
  // systemCode : 0..31
  // device : A..E
  //
  // Use third parameter to put the RC socket On or Off
  //
  // state : HIGH / LOW
  //
  actionTransmitter.sendSignal(1, rcDevice, humidifier);

}

