#include "config.h"

CRGB corner[NUM_SINGLE][LEDS_PER_STRIP_SINGLE];
CRGB grid[NUM_STRIPS][LEDS_PER_STRIP];
int cornerButtonPins[4] = {13, 4, 34, 23};
bool initiate = false;
bool lastButtonState[13] = {false};

// Server configuration - change this IP to match your server
const char* SERVER_IP = "192.168.1.68";