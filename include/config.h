#pragma once
#include <FastLED.h>

#define NUM_STRIPS 3
#define LEDS_PER_STRIP 3
#define NUM_SINGLE 4
#define LEDS_PER_STRIP_SINGLE 1

extern CRGB grid[NUM_STRIPS][LEDS_PER_STRIP];
extern CRGB corner[NUM_SINGLE][LEDS_PER_STRIP_SINGLE];
extern int cornerButtonPins[4];
extern bool initiate;
extern bool lastButtonState[13];

// Server configuration
extern const char* SERVER_IP;
extern const char* BOX_NAME;
extern const char* OTA_PASSWORD;