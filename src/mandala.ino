#include <string.h>
#include <Arduino.h>
#include <SPI.h>

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif

#include "FastLED.h"

// the pin that the LEDs are attached to
#define DATA_PIN D4

// the analog pin that the mode/brightness button is attached to
#define MODE_PIN A0
// the minimum voltage reading from A0 before a button press is recognized
#define ANALOG_THRESHOLD 900

// the number of LEDs in the outer ring
#define NUM_LEDS_RING 31
// the number of legs attached to the underside of the mandala
#define NUM_MANDALA_LEGS 4
// the number of LEDs per mandala leg
#define NUM_LEDS_MANDALA_LEG 4
// the total number of LEDs in the mandala
#define NUM_LEDS_MANDALA (NUM_MANDALA_LEGS * NUM_LEDS_MANDALA_LEG)
// the total number of LEDs in the project
#define NUM_LEDS (NUM_LEDS_RING + NUM_LEDS_MANDALA)

// 60ma/led, 2A max slip ring
#define PROJECT_MAX_BRIGHTNESS 180

// the number of program loops that must occur before brightness change occurs
#define MIN_BRIGHTNESS_LOOPS 3

// the LED strip
CRGB leds[NUM_LEDS];

#define NUM_RANDOM_COLORS 15
CRGB randomColors[NUM_RANDOM_COLORS];

#define NUM_MODES 5
enum Mode
{
  Cycle = 0,
  Chase = 1,
  SlowFade = 2,
  Fill = 3,
  StarSpin = 4,
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
Mode curMode = Mode::Cycle;
Mode cycleMode = Mode::Chase;

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop()
{
  readMode();
  if (curMode == Mode::Cycle)
  {
    displayMode(cycleMode);

    if (millis() - lastLoopMillis > 10000)
    {
      cycleMode = (Mode)((((int)cycleMode) + 1) % NUM_MODES);
    }
  }
  else
  {
    displayMode(curMode);
  }

  FastLED.show();

  lastLoopMillis = millis();
}

void readMode()
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
      const Mode newMode = (Mode)((((int)curMode) + 1) % NUM_MODES);

      handleModeChange(curMode, newMode);

      curMode = newMode;
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
}

void handleModeChange(
    Mode curMode,
    Mode newMode)
{
  Serial.printf("Changed mode from [%s] to [%s]\n", __STRINGIFY(curMode), __STRINGIFY(newMode));

  switch (newMode)
  {
  case Mode::SlowFade:
  case Mode::Fill:
    Serial.printf("Generating [%d] random colors\n", NUM_RANDOM_COLORS);

    for (int i = 0; i < NUM_RANDOM_COLORS; i++)
    {
      randomColors[i] = CRGB(random8(), random8(), random8());
    }
    break;
  default:
    Serial.printf("No custom mode setup for [%s]\n", __STRINGIFY(newMode));
    break;
  }
}

int currentRedIndex = 0;
int currentBlueIndex = NUM_LEDS_RING / 3;
int currentGreenIndex = NUM_LEDS_RING / 3 * 2;

void displayMode(
    Mode curMode)
{
  FastLED.clearData();

  switch (curMode)
  {
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
  case Mode::StarSpin:
    displayStarSpin();
    break;
  }
}

/// @brief Maps an index to its physical position
int mapPixel(
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

  return retVal;
}

void displayChase(
    CRGB chaseColor,
    int *currentChaseIndex)
{
  unsigned char nextChaseIndex = (*currentChaseIndex + 1) % NUM_LEDS;

  leds[mapPixel(nextChaseIndex)] = chaseColor;

  *currentChaseIndex = nextChaseIndex;
  delay(100);
}

void displaySlowFade()
{
  const long now = millis();
  const int fadeTimeMs = 2000;
  const unsigned long currentIteration = (now / fadeTimeMs) % NUM_RANDOM_COLORS;

  CRGB currentColor = randomColors[currentIteration];
  CRGB nextColor = randomColors[(currentIteration + 1) % NUM_RANDOM_COLORS];

  float blendAmount = 256 * (now % fadeTimeMs) / (float)fadeTimeMs;
  CRGB blended = blend(currentColor, nextColor, blendAmount);

  fill_solid(leds, NUM_LEDS, blended);

  delay(105);
}

int fillNum = 1;
int fillColorIndex = 0;

void displayFill()
{
  fill_solid(leds, fillNum++, randomColors[fillColorIndex]);

  if (fillNum >= NUM_LEDS)
  {
    fillNum = 1;
    fillColorIndex = (fillColorIndex + 1) % NUM_RANDOM_COLORS;
  }

  delay(200);
}

int starSpinLegNum = 0;
CRGB lastLegLed;

void displayStarSpin()
{
  int firstLegLed = mapPixel(NUM_LEDS_RING + (starSpinLegNum++ % NUM_MANDALA_LEGS) * NUM_LEDS_MANDALA_LEG);

  fill_rainbow(&leds[firstLegLed], NUM_LEDS_MANDALA_LEG, lastLegLed);
  lastLegLed = leds[firstLegLed + NUM_LEDS_MANDALA_LEG - 1];

  delay(250);
}
