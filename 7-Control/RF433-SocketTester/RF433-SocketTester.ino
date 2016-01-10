/*
 Control of Humidifier Connected to a RC Socket
 https://github.com/krzychb/OnlineHumidifier
 Part 7 - Control
 
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
// RF 433 transmitter configuration
//
// download - https://bitbucket.org/fuzzillogic/433mhzforarduino/wiki/Home/
//
#include <RemoteTransmitter.h>
#define RF433_TR_PIN D1   // pin where RF433 Transmitter is connected to
ActionTransmitter actionTransmitter(RF433_TR_PIN);


//
// Set up initial parameters of application
//
void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("RF433-SocketTester.ino");
}


bool outputRF;

//
// run infinite loop
//
void loop()
{
  // invert (negate) current state of outputRF variable
  outputRF = !outputRF;

  actionTransmitter.sendSignal(1, 'A', outputRF);
  
  delay(1000);
}

