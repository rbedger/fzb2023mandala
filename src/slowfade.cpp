#include "slowfade.h"
#include "constants.h"
#include "mode.h"

void SlowFadeMode::Display()
{
  const long now = millis();
  const int fadeTimeMs = 2000;
  const unsigned long currentIteration = (now / fadeTimeMs) % ModeReader::NumColors;

  CRGB currentColor = colors[currentIteration];
  CRGB nextColor = colors[(currentIteration + 1) % ModeReader::NumColors];

  float blendAmount = 256 * (now % fadeTimeMs) / (float)fadeTimeMs;
  CRGB blended = blend(currentColor, nextColor, blendAmount);

  fill_solid(leds, NUM_LEDS, blended);

  delay(105);
}
