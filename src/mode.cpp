#include "mode.h"
#include "constants.h"
#include "FastLED.h"

void ModeReader::handleModeChange(
    Mode oldMode,
    Mode newMode)
{
  Serial.printf("Changed mode from [%s] to [%s]\n", __STRINGIFY(oldMode), __STRINGIFY(newMode));

  switch (newMode)
  {
  case Mode::SlowFade:
  case Mode::Fill:
    Serial.printf("Generating [%d] random colors\n", NumColors);

    for (int i = 0; i < NumColors; i++)
    {
      randomColors[i] = util.RandomCRGB();
    }
    break;
  case Mode::Chase:
    Serial.println("Randomizing chase colors");
    chaseMode.RandomizeColors();
    break;
  default:
    Serial.printf("No custom mode setup for [%s]\n", __STRINGIFY(newMode));
    break;
  }
}

Mode ModeReader::ReadMode()
{
  int buttonReading = analogRead(MODE_PIN);
  OnOff curBtnState = buttonReading < ANALOG_THRESHOLD
                          ? OnOff::OFF
                          : OnOff::ON;

  if (curBtnState == OnOff::OFF)
  {
    // only change mode when the button is quick-pressed.
    // i.e. transitioning to OFF from a brightness change should not change mode.
    if (prevBtnState == OnOff::ON && modePressLoopCount <= MIN_BRIGHTNESS_LOOPS)
    {
      const Mode newMode = (Mode)((((int)CurrentMode) + 1) % NUM_MODES);

      handleModeChange(CurrentMode, newMode);

      CurrentMode = newMode;
    }

    modePressLoopCount = 0;
  }
  else
  {
    // if the button has been held down long enough, start changing the brightness in increments
    if (++modePressLoopCount > 3)
    {
      const unsigned long now = millis();
      const unsigned long elapsedMillis = now - lastModePressMillis;

      // debounce brightness change a bit
      if (elapsedMillis > 175)
      {
        FastLED.setBrightness((FastLED.getBrightness() + 25) % PROJECT_MAX_BRIGHTNESS);

        Serial.printf("Changed brightness to [%d]", FastLED.getBrightness());

        lastModePressMillis = now;
      }
    }
  }

  prevBtnState = curBtnState;

  return CurrentMode;
}

void ModeReader::DisplayMode(
    Mode curMode)
{
  FastLED.clearData();

  switch (curMode)
  {
  case Mode::Chase:
    chaseMode.Display();
    break;
  case Mode::SlowFade:
    slowFadeMode.Display();
    break;
  case Mode::Fill:
    fillMode.Display();
    break;
  case Mode::StarSpin:
    starSpinMode.Display();
    break;
  }
}
