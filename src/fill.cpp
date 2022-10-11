#include "fill.h"
#include "constants.h"
#include "mode.h"

void FillMode::Display()
{
  fill_solid(leds, fillNum++, colors[fillColorIndex]);

  if (fillNum >= NUM_LEDS)
  {
    fillNum = 1;
    fillColorIndex = (fillColorIndex + 1) % ModeReader::NumColors;
  }

  delay(200);
}
