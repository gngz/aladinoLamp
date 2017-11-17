/*
	aladinoLamp Firmware - Maxim DS1631 Temperature Driver Class Header File
	Defines Functions to interface with DS1631 Temperature Sensor
	Created by Gonçalo Passos (Génios da Lampada)
	
*/
#ifndef TEMPDRIVER_H
#define TEMPDRIVER_H

#include "Arduino.h"

#define STARTCONVERT 0x51
#define STOPCONVERT 0x22
#define READTEMP 0xAA
#define ACCESSTH 0xA1
#define ACCESSTL 0xA2
#define ACCESSCONFIG 0xAC

#define NINEBITRES 0x00
#define TENBITRES 0x01
#define ELEVENBITRES 0x02
#define TWELVEBITRES 0x03

class TempDriver {
private:
	byte temp_address;
	void setConfig();

public:
	TempDriver(byte address);
	void initialize(byte address);
	float getTemp();

};



#endif