/*
This is the Nanoshield library to access an external I2C EEPROM

Copyright (c) 2013 Circuitar
This software is released under the MIT license. See the attached LICENSE file for details.
*/

#ifdef ARDUPI
    #include "arduPi.h"
#else
    #include "Arduino.h"
    #include <Wire.h>
#endif

class Nanoshield_EEPROM {
	public:
		Nanoshield_EEPROM(byte a0 = 0, byte a1 = 0, byte a2 = 0, bool sba = false, int pgSize = 64);
		void begin();
		bool isReady();
		unsigned int write(unsigned int addr, byte data, unsigned int count = 1, bool retry = true);
		unsigned int write(unsigned int addr, byte data[], unsigned int len);
		bool startReading(unsigned int addr, bool retry = true);
		byte read();

	private:
		int8_t i2cAddress;
		int pageSize;
		bool singleByteAddr;

		unsigned int pageWrite(unsigned int addr, byte data[], unsigned int len, bool singleByte);
};
