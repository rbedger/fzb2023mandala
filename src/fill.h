#include "FastLED.h"

#ifndef __FILL_H__
#define __FILL_H__

class FillMode
{
public:
  FillMode(
    CRGB *_leds,
    CRGB *_colors) :
    leds(_leds),
    colors(_colors) {}

  void Display();

private:
  CRGB *leds;
  CRGB *colors;
  int fillNum = 1;
  int fillColorIndex = 0;
};

#endif
