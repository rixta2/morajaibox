#pragma once
#include <FastLED.h>
#include <Arduino.h>

enum ButtonType {
    Green,
    Red,
    Blue,
    Yellow,
    White,
    Black
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

extern Button GA1, GA2, GA3, GB1, GB2, GB3, GC1, GC2, GC3;
extern Button* gridButtons[9];
void ButtonHandler();
void setButtonTypesAndColours(ButtonType types[9], CRGB colours[9]);
void handleButtonFunction(Button* btn);