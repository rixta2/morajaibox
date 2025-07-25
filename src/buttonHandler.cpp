#include "buttonHandler.h"
#include "config.h"
#include <FastLED.h>

Button GA1 = {5, 0, 2, false, false, 0, CRGB::Black, Green};
Button GA2 = {17, 0, 1, false, false, 0, CRGB::Black, Red};
Button GA3 = {16, 0, 0, false, false, 0, CRGB::Black, Blue};
Button GB1 = {21, 1, 2, false, false, 0, CRGB::Black, Yellow};
Button GB2 = {19, 1, 1, false, false, 0, CRGB::Black, White};
Button GB3 = {18, 1, 0, false, false, 0, CRGB::Black, Green};
Button GC1 = {25, 2, 2, false, false, 0, CRGB::Black, Red};
Button GC2 = {32, 2, 1, false, false, 0, CRGB::Black, Blue};
Button GC3 = {33, 2, 0, false, false, 0, CRGB::Black, Yellow};

Button* gridButtons[9] = {&GA1, &GA2, &GA3, &GB1, &GB2, &GB3, &GC1, &GC2, &GC3};

void ButtonHandler() {
    for (int i = 0; i < 9; i++) {
        bool currentState = digitalRead(gridButtons[i]->pin);

        if (currentState != lastButtonState[i]) {
            lastButtonState[i] = currentState;

            if (currentState == LOW) { // Button pressed
                if (!gridButtons[i]->isPressed) {
                    gridButtons[i]->justPressed = true;
                    handleButtonFunction(gridButtons[i]); // Optional: handle logic by type
                }
                gridButtons[i]->isPressed = true;
            } else {
                gridButtons[i]->isPressed = false;
                gridButtons[i]->justPressed = false;
            }
        } else {
            gridButtons[i]->justPressed = false;
        }

        // Use the colour field for LED color
        grid[gridButtons[i]->row][gridButtons[i]->col] = gridButtons[i]->colour;
    }
    FastLED.show();
}

void setButtonTypesAndColours(ButtonType types[9], CRGB colours[9]) {
    for (int i = 0; i < 9; i++) {
        gridButtons[i]->type = types[i];
        gridButtons[i]->colour = colours[i];
    }
}

void handleButtonFunction(Button* btn) {
    switch (btn->type) {
        case Green: {
            // Turn this button black
            btn->colour = CRGB::Black;
            btn->type = Black;

            // Find the button to the right (wrap around)
            // Find the button to the left (wrap around)
            int row = btn->row;
            int col = btn->col;
            int prevCol = (col + 2) % 3; // (col - 1 + 3) % 3 for wrap-around
            for (int i = 0; i < 9; i++) {
                if (gridButtons[i]->row == row && gridButtons[i]->col == prevCol) {
                    gridButtons[i]->colour = CRGB::Green;
                    gridButtons[i]->type = Green;
                    break;
                }
            }
            Serial.println("Green button cycled!");
            break;
        }
        case Red:
            // Logic for red button
            Serial.println("Red button action!");
            break;
        case Blue:
            // Logic for blue button
            Serial.println("Blue button action!");
            break;
        case Yellow:
            // Logic for yellow button
            Serial.println("Yellow button action!");
            break;
        case White:
            // Logic for white button
            Serial.println("White button action!");
            break;
        case Black:
            // Logic for black button (if any)
            break;
        default:
            break;
    }
}