#include "FastLED.h"

#include "constants.h"
#include "util.h"

#ifndef __CHASE_H__
#define __CHASE_H__

class ChaseMode
{
public:
  ChaseMode(CRGB *_leds) : leds(_leds) {}

  void RandomizeColors();

  void Display();

private:
  CRGB *leds;
  Util util;

  CRGB firstColor;
  int firstIndex = 0;

  CRGB secondColor;
  int secondIndex = NUM_LEDS_RING / 3;

  CRGB thirdColor;
  int thirdIndex = NUM_LEDS_RING / 3 * 2;

  void displayChase(
      CRGB chaseColor,
      int *currentChaseIndex);
};

#endif
