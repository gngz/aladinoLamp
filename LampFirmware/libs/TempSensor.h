/*
  aladinoLamp Firmware - Maxim DS1631 Temperature Sensor Driver - Header File
  Defines Temperature Sensor Operations
  Created by Gonçalo Passos (Génios da Lampada)
  
*/

#ifndef TSENSOR_H
#define TSENSOR_H

#include "Arduino.h"

#define START_CONVERT 0x51
#define STOP_CONVERT 0x22
#define READ_TEMP 0xAA
#define ACCESS_CONFIG 0xAC
#define TEMP_CONFIG 0x04

class TempSensor {
private:
	int8_t temp_h, temp_l;
	byte temp_address;
public:
  TempSensor();
	TempSensor(byte address);
  void startConvert();
	void stopConvert();
	void setConfig();
	byte getConfig();
	float getTemp();
  bool detect();


};





#endif
