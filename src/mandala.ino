#include <string.h>
#include <Arduino.h>
#include <SPI.h>

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif

// A small helper
void error(const __FlashStringHelper *err)
{
  Serial.println(err);
  while (1)
    ;
}

float parsefloat(uint8_t *buffer);
void printHex(const uint8_t *data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];

#include "FastLED.h"

#define MODE_PIN D5
#define DATA_PIN D0
#define NUM_LEDS 31 + (4 * 4)
#define ANALOG_THRESHOLD 1000
#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS_LOOPS 3

CRGB leds[NUM_LEDS];

enum Mode
{
  Actualization = 0,
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
Mode curMode = Mode::Actualization;

CRGB solidColor = CRGB::Red;

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop()
{
  readMode();
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
        FastLED.setBrightness((FastLED.getBrightness() + 25) % MAX_BRIGHTNESS);

        Serial.print("Changed brightness to: ");
        Serial.println(FastLED.getBrightness());

        lastModePressMillis = now;
      }
    }
  }

  prevBtnState = curBtnState;
}

void displayMode()
{
  switch (curMode)
  {
  case Mode::Actualization:
    fill_solid(leds, NUM_LEDS, CRGB::White);
    break;
  }
}

CRGB slowFadeColors[NUM_LEDS] = {
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

void displaySlowFade()
{
  const int factor = 1000;
  const unsigned long iteration = (factor + millis()) / factor;

  Serial.print("Iteration: ");
  Serial.println(iteration);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    int currentColorIndex = (i + iteration) / NUM_LEDS;
    CRGB currentColor = slowFadeColors[currentColorIndex];
    CRGB nextColor = slowFadeColors[(currentColorIndex + 1) % NUM_LEDS];

    float blendAmount = (iteration % 100) / (float)100;
    CRGB blended = blend(currentColor, nextColor, blendAmount);

    if (i == 0)
    {
      Serial.println(currentColorIndex);
      Serial.println(blendAmount);
    }

    leds[i] = blended;
  }

  delay(1);
}

unsigned long lastAccelerometerUpdate = 0;
uint8_t maxX = 0, maxY = 0, maxZ = 0;

const int xLeds[4] = {0, 1, 2, 3};
const int yLeds[4] = {4, 5, 6, 8};
const int zLeds[3] = {7, 9, 10};

unsigned long getAccelerometerColor(uint8_t value)
{
  if (value < 5)
  {
    return CRGB::Red;
  }

  if (value < 10)
  {
    return CRGB::Yellow;
  }

  if (value < 15)
  {
    return CRGB::Orange;
  }

  return CRGB::White;
}
