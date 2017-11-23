/*
	aladinoLamp Firmware - Maxim DS1307 Real Time Clock Driver - Header File
	Defines RTC, Date and Time operations.
	Created by Gonçalo Passos (Génios da Lampada)
	
*/

#ifndef RTCDRIVER_H
#define RTCDRIVER_H

#include <Arduino.h>
#include <Wire.h>

#define RTC_ADDRESS 0b01101000
#define DATETIME_ADDR 0x00
#define RAM_ADDR 0x08

#define SUNDAY		1
#define MONDAY		2
#define TUESDAY		3
#define WEDNESDAY	4
#define THURSDAY	5
#define FRIDAY 		6
#define SATURDAY	7


class RTCDriver;

class DateTime {
private:

	// Variables

	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t dow;
	uint8_t day;
	uint8_t month;
	uint8_t year; // Difference between the actual year and 1970

	// Functions

public:

	// Functions

	bool setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
	bool setDate(uint8_t day, uint8_t month, int year);
	bool setDow(uint8_t dow);
	uint8_t getSeconds();
	uint8_t getMinutes();
	uint8_t getHours();
	uint8_t getDow();
	uint8_t getDay();
	uint8_t getMonth();
	int getYear();


};

class RTCDriver {
private:
	uint8_t decodeBCD(byte bcd);
	byte encodeBCD(uint8_t num);
public:
	RTCDriver();
	bool detect();
	DateTime getDateTime();
	int8_t setDateTime(DateTime t);
	int8_t writeRam(byte addr, byte* data, uint8_t size);
	uint8_t writeRam(byte addr, byte data);
	void readRam(byte addr,byte* buffer, uint8_t size);
	byte readRam(byte addr);

};
#endif
