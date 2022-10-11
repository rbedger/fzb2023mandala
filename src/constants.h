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
