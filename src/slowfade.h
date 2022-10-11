#include "FastLED.h"

#ifndef __SLOWFADE_H__
#define __SLOWFADE_H__

class SlowFadeMode
{
public:
  SlowFadeMode(
      CRGB *_leds,
      CRGB *_colors)
      : leds(_leds),
        colors(_colors) {}

  void Display();

private:
  CRGB *leds;
  CRGB *colors;
};

#endif
