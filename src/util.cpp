#include "util.h"
#include "constants.h"

/// @brief Maps an index to its physical position
int Util::MapPixel(
    int index)
{
  int retVal = 0;

  // pixels in the ring are offset by one leg
  if (index < NUM_LEDS_RING)
  {
    retVal = index + NUM_LEDS_MANDALA_LEG;
  }
  else
  {
    int legIndex = 0;
    int indexAfterRing = index - NUM_LEDS_RING;
    if (index >= NUM_LEDS_RING + NUM_LEDS_MANDALA_LEG)
    {
      legIndex = indexAfterRing / NUM_LEDS_MANDALA_LEG;
    }

    switch (legIndex)
    {
    case 0:
      // the first leg is physically before the ring, but logically after
      retVal = (index + 1) % NUM_LEDS_MANDALA_LEG;
      break;
    case 1:
    {
      // the second leg runs INWARD, across from the first leg
      // thus, it needs to reverse direction
      int endOfLeg = NUM_LEDS_RING + NUM_LEDS_MANDALA_LEG * 2 - 1;
      int offset = endOfLeg - index;
      retVal = endOfLeg - offset;
      break;
    }
    case 2:
    case 3:
      retVal = NUM_LEDS_RING + (NUM_LEDS_MANDALA_LEG * legIndex) + (indexAfterRing % NUM_LEDS_MANDALA_LEG);
      break;
    }
  }
}

CRGB Util::RandomCRGB()
{
  return CRGB(random8(), random8(), random8());
}
