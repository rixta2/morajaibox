#pragma once
#include <FastLED.h>
#include <Arduino.h>

enum ButtonType {
    Brown,
    Red,
    Blue,
    Yellow,
    White,
    Black,
    Green,
    Grey
};

struct Button {
    int pin;
    int row, col;
    bool isPressed;
    bool justPressed;
    unsigned long lastDebounceTime;
    CRGB colour;
    ButtonType type;
};

constexpr CRGB BUTTON_COLORS[] = {
    CRGB::Brown,  // Brown = 0
    CRGB::Red,    // Red = 1  
    CRGB::Blue,   // Blue = 2
    CRGB::Yellow, // Yellow = 3
    CRGB::White,  // White = 4
    CRGB::Black,  // Black = 5
    CRGB::Green,  // Green = 6
    CRGB::Gray    // Grey = 7
};

inline CRGB getButtonColor(ButtonType type) {
    return BUTTON_COLORS[static_cast<int>(type)];
}

extern Button GA1, GA2, GA3, GB1, GB2, GB3, GC1, GC2, GC3;
extern Button* gridButtons[9];
void ButtonHandler();
void setButtonTypes(ButtonType types[9]);
void handleButtonFunction(Button* btn);