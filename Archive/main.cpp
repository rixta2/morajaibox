#include <Arduino.h>
#include <FastLED.h>

#define NUM_STRIPS 3
#define LEDS_PER_STRIP 3
#define NUM_SINGLE 4
#define LEDS_PER_STRIP_SINGLE 1

boolean grid[3][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

//pin outs
/*
TL - 13
TR - 4
BL - 26
BR - 23
A1 - 5
A2 - 17
A3 - 16
B1 - 21
B2 - 19
B3 - 18
C1 - 25
C2 - 32
C3 - 33
*/

const uint8_t STRIP_PINS[NUM_STRIPS] = {2, 27, 22};
CRGB leds[NUM_STRIPS][LEDS_PER_STRIP];
const uint8_t STRIP_PINS2[NUM_SINGLE] = {12, 15, 3, 14};
CRGB leds2[NUM_SINGLE][LEDS_PER_STRIP_SINGLE];

const uint8_t BUTTON_PINS[13] = {4, 16, 17, 5, 18, 19, 21, 13, 23, 32, 33, 25, 26};
bool lastButtonState[13] = {false};

void setup() {
  Serial.begin(115200);

  //Grid LEDs
  FastLED.addLeds<WS2812B, 27, GRB>(leds[0], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 2, GRB>(leds[1], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 22, GRB>(leds[2], LEDS_PER_STRIP);

  //Corner LEDs
  FastLED.addLeds<WS2812B, 12, GRB>(leds2[0], LEDS_PER_STRIP_SINGLE);
  FastLED.addLeds<WS2812B, 15, GRB>(leds2[1], LEDS_PER_STRIP_SINGLE);
  FastLED.addLeds<WS2812B, 3, GRB>(leds2[2], LEDS_PER_STRIP_SINGLE);
  FastLED.addLeds<WS2812B, 14, GRB>(leds2[3], LEDS_PER_STRIP_SINGLE);

  FastLED.setBrightness(128);
  delay(100);

  // Initialize button pins
  for (int i = 0; i < 13; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  // LEDs off at start
  //some pins are pulled high on device start, change pinouts in future revision
  for (int strip = 0; strip < NUM_STRIPS; strip++) {
    for (int led = 0; led < LEDS_PER_STRIP; led++) {
      leds[strip][led] = CRGB::Black;
    }
  }
  for (int strip = 0; strip < NUM_SINGLE; strip++) {
    for (int led = 0; led < LEDS_PER_STRIP_SINGLE; led++) {
      leds2[strip][led] = CRGB::Black;
    }
  }
  FastLED.show();
}

void buttonHandler() {
  // Check each button state
  for (int i = 0; i < 13; i++) {
    bool currentState = digitalRead(BUTTON_PINS[i]);
    if (currentState != lastButtonState[i]) {
      lastButtonState[i] = currentState;
      if (currentState == LOW) { // Button pressed
        Serial.printf("Button %d pressed\n", BUTTON_PINS[i]);
        // Handle button press logic here
      }
    }
  }
}

void turnSingleLEDsOffOnce() {
  static bool done = false;
  if (!done) {
    for (int strip = 0; strip < NUM_SINGLE; strip++) {
      leds2[strip][0] = CRGB::Black;
    }
    FastLED.show();
    done = true;
  }
}

void loop() {
  turnSingleLEDsOffOnce();
  buttonHandler();
  delay(10);
}