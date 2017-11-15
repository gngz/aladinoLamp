/*
	aladinoLamp Firmware Led Class Header File
	Defines Color and Led Datatypes and functions to manipulate leds.
	Created by Gonçalo Passos (Génios da Lampada)
	
*/

#ifndef LED_H
#define LED_H

#include "Arduino.h"

struct color
{
	char red;
	char green;
	char blue;
	char intensity;
};

class Led {


public:

	// Public Function Prototypes

	void setColor(color c);
	void setPins(char r, char g, char b);
	color getColor();

private:
	color colour;
	char pin_r;
	char pin_g;
	char pin_b;

};

#endif