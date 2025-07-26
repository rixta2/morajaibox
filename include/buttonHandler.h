#pragma once
#include <FastLED.h>
#include <Arduino.h>

enum ButtonType {
    Pink,
    Red,
    Orange,
    Blue,
    Violet,
    Yellow,
    White,
    Black,
    Brown,
    Green
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
    CRGB::DeepPink,  // Pink = 0
    CRGB::Red,       // Red = 1  
    CRGB::Orange,    // Orange = 2
    CRGB::Blue,      // Blue = 3
    CRGB::Violet,    // Violet = 4
    CRGB::Yellow,    // Yellow = 5
    CRGB::White,     // White = 6
    CRGB::Black,     // Black = 7
    CRGB::Brown,     // Brown = 8
    CRGB::Green      // Green = 9
};

inline CRGB getButtonColor(ButtonType type) {
    return BUTTON_COLORS[static_cast<int>(type)];
}

extern Button GA1, GA2, GA3, GB1, GB2, GB3, GC1, GC2, GC3;
extern Button* gridButtons[9];
void ButtonHandler();
void setButtonTypes(ButtonType types[9]);
void handleButtonFunction(Button* btn);
void executeButtonBehavior(ButtonType behaviorType, Button* fromPosition);