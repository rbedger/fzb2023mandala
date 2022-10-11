#include "chase.h"
#include "constants.h"

void ChaseMode::RandomizeColors() {
  firstColor = util.RandomCRGB();
  secondColor = util.RandomCRGB();
  thirdColor = util.RandomCRGB();
}

void ChaseMode::Display()
{
  displayChase(
      firstColor,
      &firstIndex);

  displayChase(
      secondColor,
      &secondIndex);

  displayChase(
      thirdColor,
      &thirdIndex);
}

void ChaseMode::displayChase(
    CRGB chaseColor,
    int *currentChaseIndex)
{
  unsigned char nextChaseIndex = (*currentChaseIndex + 1) % NUM_LEDS;

  leds[util.MapPixel(nextChaseIndex)] = chaseColor;

  *currentChaseIndex = nextChaseIndex;
  delay(100);
}
