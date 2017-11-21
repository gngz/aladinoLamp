/*
	aladinoLamp Firmware Led Class File
	Defines Color and Led Datatypes and functions to manipulate leds.
	Created by Gonçalo Passos (Génios da Lampada)
	
*/

#include "Arduino.h"
#include "Led.h"


void Led::setPins(uint8_t r, uint8_t g, uint8_t b){

	pin_r = r;
	pin_g = g;
	pin_b = b;

	// NodeMCU Dependent Code

	pinMode(r,OUTPUT);
	pinMode(g,OUTPUT);
	pinMode(b,OUTPUT);

}

void Led::setColor(color c){

	uint8_t red;
	uint8_t green;
	uint8_t blue;
	float iten;

	// Save color

	colour.red = c.red;
	colour.green = c.green;
	colour.blue = c.blue;
	colour.intensity = c.intensity;

	// Calculate Color by itensity

	iten = (float) c.intensity / 100;
	red = c.red * iten;
	green = c.green * iten;
	blue = c.blue * iten;

	// NodeMCU Dependent Code

	analogWrite(pin_r,red);
	analogWrite(pin_g,green);
	analogWrite(pin_b,blue);

}


void Led::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t intensity){

	uint8_t red;
	uint8_t green;
	uint8_t blue;
	float iten;

	// Save color

	colour.red = r;
	colour.green = g;
	colour.blue = b;
	colour.intensity = intensity;
	// Calculate Color by itensity

	iten = (float) intensity / 100;
	red = r * iten;
	green = g * iten;
	blue = b * iten;

	// NodeMCU Dependent Code

	analogWrite(pin_r,red);
	analogWrite(pin_g,green);
	analogWrite(pin_b,blue);

}

color Led::getColor()
{
	return colour;
}

Led::Led(uint8_t r, uint8_t g, uint8_t b)
{
	setPins(r,g,b);
}
