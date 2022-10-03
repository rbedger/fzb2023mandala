#include <string.h>
#include <Arduino.h>
#include <SPI.h>

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif

#include "FastLED.h"

#define DATA_PIN D0

#define MODE_PIN D5
#define ANALOG_THRESHOLD 1000

#define NUM_LEDS_RING 31
#define NUM_MANDALA_LEGS 4
#define NUM_LEDS_MANDALA_LEG 4
#define NUM_LEDS_MANDALA (NUM_MANDALA_LEGS * NUM_LEDS_MANDALA_LEG)
#define NUM_LEDS (NUM_LEDS_RING + NUM_LEDS_MANDALA)

// 60ma/led, 2A max slip ring
#define PROJECT_MAX_BRIGHTNESS 180

#define MIN_BRIGHTNESS_LOOPS 3

CRGB leds[NUM_LEDS];

enum Mode
{
  Boot = 0,
  Chase = 1,
  SlowFade = 2,
  Fill = 3,
};

enum OnOff
{
  ON,
  OFF,
};

uint8_t modePressLoopCount = 0;
unsigned long lastModePressMillis = 0;
unsigned long lastLoopMillis = 0;

OnOff prevBtnState = OnOff::OFF;
Mode curMode = Mode::Fill;

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop()
{
  // readMode();
  displayMode();

  FastLED.show();

  lastLoopMillis = millis();
}

void readMode()
{
  OnOff curBtnState = analogRead(MODE_PIN) < ANALOG_THRESHOLD
                          ? OnOff::OFF
                          : OnOff::ON;

  if (curBtnState == OnOff::OFF)
  {
    // only change mode when the button is quick-pressed.
    // i.e. transitioning to OFF from a brightness change should not change mode.
    if (prevBtnState == OnOff::ON && modePressLoopCount <= MIN_BRIGHTNESS_LOOPS)
    {
      curMode = (Mode)((((int)curMode) + 1) % 1);

      Serial.print("Mode switched to ");
      Serial.println(curMode);
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
      Serial.print("Elapsed: ");
      Serial.println(elapsedMillis);

      // debounce brightness change a bit
      if (elapsedMillis > 175)
      {
        FastLED.setBrightness((FastLED.getBrightness() + 25) % PROJECT_MAX_BRIGHTNESS);

        Serial.print("Changed brightness to: ");
        Serial.println(FastLED.getBrightness());

        lastModePressMillis = now;
      }
    }
  }

  prevBtnState = curBtnState;
}

int currentRedIndex = 0;
int currentBlueIndex = NUM_LEDS_RING / 3;
int currentGreenIndex = NUM_LEDS_RING / 3 * 2;

void displayMode()
{
  FastLED.clearData();

  switch (curMode)
  {
  case Mode::Boot:
    fill_solid(leds, NUM_LEDS, CRGB::White);
    break;
  case Mode::Chase:
    displayChase(
        CRGB::Red,
        &currentRedIndex);

    displayChase(
        CRGB::Blue,
        &currentBlueIndex);

    displayChase(
        CRGB::Green,
        &currentGreenIndex);

    break;
  case Mode::SlowFade:
    displaySlowFade();
    break;
  case Mode::Fill:
    displayFill();
    break;
  }
}

void displayChase(
    CRGB chaseColor,
    int *currentChaseIndex)
{
  unsigned char nextChaseIndex = (*currentChaseIndex + 1) % NUM_LEDS;

  if (nextChaseIndex < NUM_LEDS_RING)
  {
    leds[nextChaseIndex] = chaseColor;
  }
  else if (nextChaseIndex < NUM_LEDS_MANDALA_LEG + NUM_LEDS_RING)
  {
    for (int i = 0; i < 4; i++)
    {
      int legIndex = nextChaseIndex + (NUM_LEDS_MANDALA_LEG * i);
      if (i % 2 == 1)
      {
        leds[legIndex] = chaseColor;
      }
      else
      {
        int endOfLeg = NUM_LEDS_RING + NUM_LEDS_MANDALA_LEG - 1 + (NUM_LEDS_MANDALA_LEG * i);
        int offset = endOfLeg - legIndex;
        leds[endOfLeg - (NUM_LEDS_MANDALA_LEG - 1 - offset)] = chaseColor;
      }
    }
  }
  else
  {
    nextChaseIndex = 0;
  }

  *currentChaseIndex = nextChaseIndex;
  delay(100);
}

CRGB slowFadeColors[] = {
    CRGB::Red,
    CRGB::Violet,
    CRGB::OrangeRed,
    CRGB::Yellow,
    CRGB::Blue,
    CRGB::YellowGreen,
    CRGB::Teal,
    CRGB::Green,
    CRGB::Orange,
    CRGB::BlueViolet,
    CRGB::Indigo,
};

const int numFadeColors = sizeof(slowFadeColors) / sizeof(*slowFadeColors);

void displaySlowFade()
{
  const long now = millis();
  const int fadeTimeMs = 2000;
  const unsigned long currentIteration = (now / fadeTimeMs) % numFadeColors;

  for (int i = 0; i < NUM_LEDS; i++)
  {
    CRGB currentColor = slowFadeColors[currentIteration];
    CRGB nextColor = slowFadeColors[(currentIteration + 1) % numFadeColors];

    float blendAmount = 100 * (now % fadeTimeMs) / (float)fadeTimeMs;
    CRGB blended = blend(currentColor, nextColor, blendAmount);

    leds[i] = blended;
  }

  delay(105);
}

int fillNum = 1;
int fillColorIndex = 0;

void displayFill()
{
  fill_solid(leds, fillNum++, slowFadeColors[fillColorIndex]);

  if (fillNum >= NUM_LEDS)
  {
    fillNum = 0;
    fillColorIndex = (fillColorIndex + 1) % numFadeColors;
  }

  delay(200);
}
