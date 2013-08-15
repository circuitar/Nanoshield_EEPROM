/*
This is an EEPROM test for the RTCMem Nanoshield using the EEPROM library.

Copyright (c) 2013 Circuitar
This software is released under the MIT license. See the attached LICENSE file for details.
*/

#include <Wire.h>
#include "Nanoshield_EEPROM.h"

// EEPROM size in bytes = 32KB = 2^15
#define EEPROM_SIZE 0x8000

Nanoshield_EEPROM eeprom(0, 0, 0);

void setup() {
  Serial.begin(9600);
  Serial.println("------------------------");
  Serial.println(" Nanoshield EEPROM Test");
  Serial.println("------------------------");
  Serial.println("");

  // Initialize EEPROM
  eeprom.begin();
  
  writeEEPROM(0xAA);
  verifyEEPROM(0xAA);
  writeEEPROM(0x55);
  verifyEEPROM(0x55);
  writeEEPROMCounter();
  verifyEEPROMCounter();

  Serial.println("");
  Serial.println("EEPROM test finished");
}

void loop() {}

boolean writeEEPROM(byte b)
{
  unsigned int i;
  unsigned long t0, t1;

  Serial.print("Filling EEPROM with 0x");
  Serial.print(b, HEX);
  Serial.print(" ... ");
  t0 = millis();
  if (eeprom.write(i, b, EEPROM_SIZE) != EEPROM_SIZE) {
    Serial.println("Failed");
    return false;
  }
  t1 = millis();
  Serial.print("OK (");
  Serial.print((t1 - t0) / 1000.0);
  Serial.println("s)");
  
  return true;
}

boolean writeEEPROMCounter()
{
  unsigned int i, n;
  unsigned long t0, t1;
  byte buf[256];
  
  // Prepare buffer to do paged writes
  for (i = 0; i < 256; i++) {
    buf[i] = i;
  }

  Serial.print("Filling EEPROM with 8-bit counter... ");
  t0 = millis();
  i = 0;
  while (i < EEPROM_SIZE) {
    if (i + 256 > EEPROM_SIZE) {
      n = EEPROM_SIZE - 1;
    } else {
      n = 256;
    }
    if (eeprom.write(i, buf, n) != n) {
      Serial.println("Failed");
      return false;
    }
    i += n;
  }
  t1 = millis();
  Serial.print("OK (");
  Serial.print((t1 - t0) / 1000.0);
  Serial.println("s)");
  
  return true;
}

boolean verifyEEPROM(byte b)
{
  unsigned int i;
  unsigned long t0, t1;
  byte c;

  Serial.print("Verifying EEPROM for 0x");
  Serial.print(b, HEX);
  Serial.print(" ... ");
  t0 = millis();
  if (!eeprom.startReading(0)) {
    Serial.println("Failed to start reading data");
    return false;
  }
  for (i = 0; i < EEPROM_SIZE; i++) {
    c = eeprom.read();
    if (c != b) {
      readFailure(i, b, c);
      return false;
    }
  }
  t1 = millis();
  Serial.print("OK (");
  Serial.print((t1 - t0) / 1000.0);
  Serial.println("s)");
  
  return true;
}

boolean verifyEEPROMCounter()
{
  unsigned int i;
  unsigned long t0, t1;
  byte c;

  Serial.print("Verifying EEPROM for 8-bit counter... ");
  t0 = millis();
  if (!eeprom.startReading(0)) {
    Serial.println("Failed to start reading data");
    return false;
  }
  for (i = 0; i < EEPROM_SIZE; i++) {
    c = eeprom.read();
    if (c != (byte)i) {
      readFailure(i, (byte)i, c);
      return false;
    }
  }
  t1 = millis();
  Serial.print("OK (");
  Serial.print((t1 - t0) / 1000.0);
  Serial.println("s)");
  
  return true;
}

void readFailure(unsigned int addr, byte expected, byte received)
{
  Serial.print("Failed at address 0x");
  Serial.print(addr, HEX);
  Serial.print(": expected 0x");
  Serial.print(expected, HEX);
  Serial.print(" but received 0x");
  Serial.println(received, HEX);
}
