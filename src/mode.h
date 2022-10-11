#define NUM_MODES 5

#include "util.h"
#include "chase.h"
#include "slowfade.h"
#include "fill.h"
#include "starspin.h"

#ifndef __MODEREADER_H__
#define __MODEREADER_H__

enum OnOff
{
  ON,
  OFF,
};

enum Mode
{
  Cycle = 0,
  Chase = 1,
  SlowFade = 2,
  Fill = 3,
  StarSpin = 4,
};

class ModeReader
{
public:
  static const int NumColors = 15;

  Mode CurrentMode = Mode::Cycle;
  Mode CycleMode = Mode::Chase;

  ModeReader(CRGB *_leds)
      : leds(_leds),
        chaseMode(ChaseMode(_leds)),
        slowFadeMode(SlowFadeMode(_leds, randomColors)),
        fillMode(FillMode(_leds, randomColors)),
        starSpinMode(StarSpinMode(_leds)) {}

  Mode ReadMode();

  void DisplayMode(
      Mode mode);

private:
  void handleModeChange(
      Mode curMode,
      Mode newMode);

  CRGB *leds;
  CRGB randomColors[ModeReader::NumColors];

  uint8_t modePressLoopCount = 0;
  unsigned long lastModePressMillis = 0;

  OnOff prevBtnState = OnOff::OFF;

  Util util;
  ChaseMode chaseMode;
  SlowFadeMode slowFadeMode;
  FillMode fillMode;
  StarSpinMode starSpinMode;
};

#endif
