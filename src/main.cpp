//header files
#include "config.h"
#include "LEDcontroller.h"
#include "puzzleSetup.h"
#include "buttonHandler.h"
#include "puzzleLogic.h"
#include "webServer.h"
#include <Arduino.h>
#include <FastLED.h>

volatile puzzleState currentState = INITIALISE;


#define NUM_STRIPS 3
#define LEDS_PER_STRIP 3

#define NUM_SINGLE 4
#define LEDS_PER_STRIP_SINGLE 1
const uint8_t STRIP_PINS2[NUM_SINGLE] = {12, 15, 3, 14};

#define BRIGHTNESS 255

void LEDSetup() {
    //grid
    FastLED.addLeds<WS2812B, 22, GRB>(grid[0], LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, 27, GRB>(grid[1], LEDS_PER_STRIP);
    FastLED.addLeds<WS2812B, 2, GRB>(grid[2], LEDS_PER_STRIP);

    //corner LEDs
    FastLED.addLeds<WS2812B, 12, GRB>(corner[0], LEDS_PER_STRIP_SINGLE);
    FastLED.addLeds<WS2812B, 15, GRB>(corner[1], LEDS_PER_STRIP_SINGLE);
    FastLED.addLeds<WS2812B, 26, GRB>(corner[2], LEDS_PER_STRIP_SINGLE);
    FastLED.addLeds<WS2812B, 14, GRB>(corner[3], LEDS_PER_STRIP_SINGLE);

    FastLED.setBrightness(BRIGHTNESS);

    // Allow time for the LEDs to initialize
    delay(100); 

    //Grid LEDs off
    for (int strip = 0; strip < NUM_STRIPS; strip++) {
      for (int led = 0; led < LEDS_PER_STRIP; led++) {
        grid[strip][led] = CRGB::Black;
      }
    }

    //corner LEDs off
    for (int strip = 0; strip < NUM_SINGLE; strip++) {
      for (int led = 0; led < LEDS_PER_STRIP_SINGLE; led++) {
        corner[strip][led] = CRGB::Black;
      }
    }
    FastLED.show();
    initiate = true;
}


void buttonSetup() {
  pinMode(GA1.pin, INPUT_PULLUP);
  pinMode(GA2.pin, INPUT_PULLUP);
  pinMode(GA3.pin, INPUT_PULLUP);
  pinMode(GB1.pin, INPUT_PULLUP);
  pinMode(GB2.pin, INPUT_PULLUP);
  pinMode(GB3.pin, INPUT_PULLUP);
  pinMode(GC1.pin, INPUT_PULLUP);
  pinMode(GC2.pin, INPUT_PULLUP);
  pinMode(GC3.pin, INPUT_PULLUP);
  
  // Setup corner buttons
  for (int i = 0; i < 4; i++) {
    pinMode(cornerButtonPins[i], INPUT_PULLUP);
  }
}

void setup() {
  Serial.begin(115200);
  LEDSetup();
  buttonSetup();
  
  // Initialize WiFi - Replace with your credentials
  initWiFi("gusmanor", "Bigboygus");
  initWebServer();
}

void loop() {
  //could this be a switch case?

  if (currentState == INITIALISE) {
    initiatePuzzle();
    currentState = PLAYING;
  }

  if (currentState == PLAYING) {
    ButtonHandler();
    
    // Update corner LEDs based on current grid state
    updateCornerLEDs();
    
    // Check corner buttons for win condition
    for (int i = 0; i < 4; i++) {
      bool currentButtonState = digitalRead(cornerButtonPins[i]) == LOW;
      bool lastState = lastButtonState[9 + i]; // Corner buttons use indices 9-12
      
      if (currentButtonState && !lastState) { // Button just pressed
        handleCornerButtonPress(i);
      }
      
      lastButtonState[9 + i] = currentButtonState;
    }
    
    // Broadcast status updates
    broadcastStatus();
  }

  if (currentState == SOLVED) {
    //handleButtons();
    //updateLEDs();
  }

  if (currentState == RESET) {
    //handleButtons();
    //updateLEDs();
  }

}


