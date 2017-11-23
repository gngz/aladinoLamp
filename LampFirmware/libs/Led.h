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
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t intensity;
};

class Led {


public:

  // Public Function Prototypes
  Led();
  Led(uint8_t r, uint8_t g, uint8_t b);
  void setColor(color c);
  void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t intensity);
  void setPins(uint8_t r, uint8_t g, uint8_t b);
  color getColor();

private:
  color colour;
  uint8_t pin_r;
  uint8_t pin_g;
  uint8_t pin_b;

};

#endif
