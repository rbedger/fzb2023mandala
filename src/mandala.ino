#include <Arduino.h>
#include "FastLED.h"

#include "mode.h"
#include "constants.h"

// the LED strip
CRGB leds[NUM_LEDS];

ModeReader mode = ModeReader(leds);

unsigned long lastLoopMillis = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop()
{
  mode.ReadMode();

  if (mode.CurrentMode == Mode::Cycle)
  {
    mode.DisplayMode(mode.CycleMode);

    if (millis() - lastLoopMillis > 10000)
    {
      mode.CycleMode = (Mode)((((int)mode.CycleMode) + 1) % NUM_MODES);
    }
  }
  else
  {
    mode.DisplayMode(mode.CurrentMode);
  }

  FastLED.show();

  lastLoopMillis = millis();
}
