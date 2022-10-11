#include "FastLED.h"

#ifndef __UTIL_H__
#define __UTIL_H__

class Util
{
public:
  int MapPixel(int logicalIndex);
  CRGB RandomCRGB();
};

#endif
