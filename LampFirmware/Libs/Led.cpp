/*
	aladinoLamp Firmware Led Class File
	Defines Color and Led Datatypes and functions to manipulate leds.
	Created by Gonçalo Passos (Génios da Lampada)
	
*/

#include "Arduino.h"
#include "Led.h"


void Led::setPins(char r, char g, char b){

	pin_r = r;
	pin_g = g;
	pin_b = b;

	// NodeMCU Dependent Code

	pinMode(r,OUTPUT);
	pinMode(g,OUTPUT);
	pinMode(b,OUTPUT);

}

void Led::setColor(color c){

	char red;
	char green;
	char blue;
	float iten;

	// Save color

	colour.red = c.red;
	colour.green = c.green;
	colour.blue = c.blue;
	colour.intensity = c.intensity;

	// Calculate Color by itensity

	iten = c.intensity / 100;
	red = c.red * iten;
	green = c.green * iten;
	blue = c.blue * iten;

	// NodeMCU Dependent Code

	analogWrite(pin_r,red);
	analogWrite(pin_g,green);
	analogWrite(pin_b,blue);

}