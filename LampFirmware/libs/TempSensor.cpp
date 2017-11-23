/*
  aladinoLamp Firmware - Maxim DS1631 Temperature Sensor Driver - Code File
  Defines Temperature Sensor Operations
  Created by Gonçalo Passos (Génios da Lampada)
  
*/

#include "Arduino.h"
#include "TempSensor.h"
#include <Wire.h>

TempSensor::TempSensor()
{
  
}

TempSensor::TempSensor(byte address){
	temp_address = address;
	Wire.begin();

}

void TempSensor::startConvert(){
	Wire.beginTransmission(temp_address);
  	Wire.write(START_CONVERT);
  	Wire.endTransmission();
}

void TempSensor::stopConvert(){
	Wire.beginTransmission(temp_address);
  	Wire.write(START_CONVERT);
  	Wire.endTransmission();
}

byte TempSensor::getConfig(){
  Wire.beginTransmission(temp_address);
  Wire.write(ACCESS_CONFIG);
  Wire.endTransmission();
  Wire.requestFrom(temp_address,1);
  Wire.available();
  return Wire.read();
}

void TempSensor::setConfig(){
	Wire.beginTransmission(temp_address);
	Wire.write(ACCESS_CONFIG);
	Wire.write(TEMP_CONFIG);
	Wire.endTransmission();	
}

float TempSensor::getTemp(){
	float temp;
  	Wire.beginTransmission(temp_address);
  	Wire.write(READ_TEMP);
  	Wire.endTransmission();
  	Wire.requestFrom(temp_address,2);
  	Wire.available();
  	temp_h = Wire.read();
  	Wire.available();
  	temp_l = Wire.read();
  	temp_l = temp_l >> 6;

  	temp = temp_h;
  	if(temp_h>= 0x80)
  	{
  	 return  temp = (temp_h * -1) - (temp_l * 0.25);
  	}
  
 	 return  temp + (float) (temp_l * 0.25);
}

bool TempSensor::detect() {
  byte code;
  Wire.beginTransmission(temp_address);
  code = Wire.endTransmission();
  return (code == 0);
}
