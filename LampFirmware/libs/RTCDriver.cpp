/*
	aladinoLamp Firmware - Maxim DS1307 Real Time Clock Driver - Code File
	Defines RTC, Date and Time operations.
	Created by Gonçalo Passos (Génios da Lampada)
	
*/

#include "RTCDriver.h"
#include <Wire.h>



bool DateTime::setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
	if(hours>=24)
		return false;
	if(minutes>59)
		return false;
	if(seconds>59)
		return false;

	this->hours = hours;
	this->minutes = minutes;
	this->seconds = seconds;

	return true;
}

bool DateTime::setDate(uint8_t day, uint8_t month, int year) {
	if(day>31)
		return false;
	if(month>12)
		return false;

	this->day = day;
	this->month = month;
	this->year = year - 1970;

	return true;
}

bool DateTime::setDow(uint8_t dow) {
	if(dow > SATURDAY)
		return false;

	this->dow = dow;

	return true;
}


uint8_t DateTime::getSeconds() {
	return this->seconds;
}

uint8_t DateTime::getMinutes() {
	return this->minutes;
}

uint8_t DateTime::getHours() {
	return this->hours;
}

uint8_t DateTime::getDow() {
	return this->dow;
}

uint8_t DateTime::getDay() {
	return this->day;
}

uint8_t DateTime::getMonth() {
	return this->month;
}

int DateTime::getYear() {
	return this->year + 1970;
}



RTCDriver::RTCDriver() {
	Wire.begin();
}

uint8_t RTCDriver::decodeBCD(byte bcd) {
	return ((bcd/16 * 10) + (bcd % 16));
}

byte RTCDriver::encodeBCD(uint8_t num) {
	return ((num/10 * 16) + (num % 10));
}

bool RTCDriver::detect() {
	byte code;
	Wire.beginTransmission(RTC_ADDRESS);
	code = Wire.endTransmission();
	return (code == 0);
}

DateTime RTCDriver::getDateTime() {
	DateTime newdate;
	byte seconds, minutes,hours,days,weekday,months,years;
	  
	Wire.beginTransmission(RTC_ADDRESS);
	Wire.write(DATETIME_ADDR);
	Wire.endTransmission();
	Wire.requestFrom(RTC_ADDRESS,7);
	  
	Wire.available();
	seconds = Wire.read();

	Wire.available();
	minutes = Wire.read();

	Wire.available();
	hours = Wire.read();

	Wire.available();
	weekday = Wire.read();

	Wire.available();
	days = Wire.read();

	Wire.available();
	months = Wire.read();

	Wire.available();
	years = Wire.read();

	seconds = decodeBCD(seconds & 0x7F);
	minutes = decodeBCD(minutes);
	hours = decodeBCD(hours & 0x3F);
	weekday = decodeBCD(weekday);
	days = decodeBCD(days);
	months = decodeBCD(months);
	years = decodeBCD(years)+2000;
	
	newdate.setTime(hours,minutes,seconds);
	newdate.setDate(days,months,years);
	newdate.setDow(weekday);

	 


	return newdate;
}

int8_t RTCDriver::setDateTime(DateTime t) {
	Wire.beginTransmission(RTC_ADDRESS);
  	Wire.write(DATETIME_ADDR);
	Wire.write(encodeBCD(t.getSeconds()));
	Wire.write(encodeBCD(t.getMinutes()));
	Wire.write(encodeBCD(t.getHours()));
	Wire.write(encodeBCD(t.getDow()));
	Wire.write(encodeBCD(t.getDay()));
	Wire.write(encodeBCD(t.getMonth()));
	Wire.write(encodeBCD(t.getYear() - 2000));
	return Wire.endTransmission();
}

int8_t RTCDriver::writeRam(byte addr, byte* data, uint8_t size) {
	if(RAM_ADDR + addr + size > 0x3F)
    	return -1;
    
    Wire.beginTransmission(RTC_ADDRESS);
    Wire.write(RAM_ADDR + addr);
 
    for(int i = 0; i<size; i++)
    {  
      Wire.write(data[i]);
    }
    Wire.endTransmission();
    return size;
}

uint8_t RTCDriver::writeRam(byte addr, byte data) {
  	return writeRam(addr, &data, 1);
}

void RTCDriver::readRam(byte addr,byte* buffer, uint8_t size ) {
  
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(RAM_ADDR + addr);
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDRESS,size);

  for(int i = 0;Wire.available() && i <size;i++)
  {
    buffer[i] = Wire.read();
  }


 
}

byte RTCDriver::readRam(byte addr) {
  byte data;
  readRam(addr,&data,1);
  return data;
}
