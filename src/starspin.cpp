#include "starspin.h"
#include "constants.h"

void StarSpinMode::Display()
{
  int firstLegLed = util.MapPixel(NUM_LEDS_RING + (starSpinLegNum++ % NUM_MANDALA_LEGS) * NUM_LEDS_MANDALA_LEG);

  fill_rainbow(&leds[firstLegLed], NUM_LEDS_MANDALA_LEG, lastLegLed);
  lastLegLed = leds[firstLegLed + NUM_LEDS_MANDALA_LEG - 1];

  delay(250);
}
