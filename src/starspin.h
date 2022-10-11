#include "FastLED.h"
#include "util.h"

#ifndef __STARSPIN_H__
#define __STARSPIN_H__

class StarSpinMode
{
public:
  StarSpinMode(CRGB *_leds) : leds(_leds) {}

  void Display();

private:
  CRGB *leds;
  Util util;
  int starSpinLegNum = 0;
  CRGB lastLegLed;
};

#endif
