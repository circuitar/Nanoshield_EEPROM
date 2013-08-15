/*
This is the Nanoshield library to access an external I2C EEPROM

Copyright (c) 2013 Circuitar
This software is released under the MIT license. See the attached LICENSE file for details.
*/

#include "Arduino.h"
#include <Wire.h>
#include "Nanoshield_EEPROM.h"

#define EEPROM_I2C_ADDRESS 0x50
#define RETRIES 8
#define DELAY_uS 1000
#define MAX_I2C_DATA 30

Nanoshield_EEPROM::Nanoshield_EEPROM(byte a0, byte a1, byte a2, bool sba, int pgSize) {
	// Select the three LSBs of the EEPROM address in the I2C bus
	i2cAddress = EEPROM_I2C_ADDRESS;
	if (a0) i2cAddress |= 0x01;
	if (a1) i2cAddress |= 0x02;
	if (a2) i2cAddress |= 0x04;
	
	// Store the page size to use during page write
	pageSize = pgSize;
	
	// Store the addressing mode
	singleByteAddr = sba;
}

void Nanoshield_EEPROM::begin()
{
	// Initiate the Wire library and join the I2C bus as a master
  Wire.begin();
}

bool Nanoshield_EEPROM::isReady()
{
	return startReading(0, false);
}

unsigned int Nanoshield_EEPROM::write(unsigned int addr, byte data, unsigned int count, bool retry)
{
	// If we need to send multiple bytes, do a paged write
	if (count > 1) return pageWrite(addr, &data, count, true);

	// Write one byte to the EEPROM with retries
	for (int i = 0; i <= RETRIES; i++) {
		Wire.beginTransmission(i2cAddress);
		if (!singleByteAddr) {
			Wire.write((addr >> 8) & 0xff);
		}
		Wire.write(addr & 0xff);
		Wire.write(data);
		if (Wire.endTransmission() == 0) return 1;
		if (!retry) break;
		delayMicroseconds(DELAY_uS);
	}

	// Exceeded number of retries
	return 0;
}

unsigned int Nanoshield_EEPROM::write(unsigned int addr, byte data[], unsigned int len)
{
	return pageWrite(addr, data, len, false);
}

bool Nanoshield_EEPROM::startReading(unsigned int addr, bool retry)
{
  // Do a dummy write to set the address
  // Will return true if the module is ready
	for (int i = 0; i <= RETRIES; i++) {
		Wire.beginTransmission(i2cAddress);
		if (!singleByteAddr) {
			Wire.write((addr >> 8) & 0xff);
		}
		Wire.write(addr & 0xff);
		if (Wire.endTransmission() == 0) return true;
		if (!retry) break;
		delayMicroseconds(DELAY_uS);
	}

	// Exceeded number of retries
	return false;
}

byte Nanoshield_EEPROM::read()
{
  Wire.requestFrom(i2cAddress, 1);
  return Wire.read();
}

unsigned int Nanoshield_EEPROM::pageWrite(unsigned int addr, byte data[], unsigned int len, bool singleByte)
{
  unsigned int end = addr + len;
	unsigned int i = 0;
	unsigned int j, n;
  
  // Write a sequence of *len* bytes to the EEPROM off of *data* using page writes
  while (addr < end) {
		// Find out the end of the current page
		n = pageSize - addr % pageSize;
		if (addr + n > end) n = end - addr;
		
		// Limit the write sizeas to not exceed the Wire library buffer size
		if (n > MAX_I2C_DATA) n = MAX_I2C_DATA;

		// Wait until module is ready
		if (!startReading(addr)) return i;

		// Do a page write
		Wire.beginTransmission(i2cAddress);
		if (!singleByteAddr) {
			Wire.write((addr >> 8) & 0xff);
		}
		Wire.write(addr & 0xff);
		if (singleByte) {
			for (j = 0; j < n; j++) {
				Wire.write(data[0]);
			}
		} else {
			Wire.write(&data[i], n);
		}
		if (Wire.endTransmission())	return i;
		
		// Continue writing on the next page
		i += n;
		addr += n;
	}

	return i;
}
