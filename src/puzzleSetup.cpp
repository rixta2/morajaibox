#include "puzzleSetup.h"
#include "buttonHandler.h"

void initiatePuzzle() {
    // Define the types for each button (order matches gridButtons)
    ButtonType types[9] = {
        Black, Green, Black,
        Black, Black, Green,
        Black, Black, Black
    };

    // Define the initial colours for each button
    CRGB colours[9] = {
        CRGB::Black, CRGB::Green, CRGB::Black,
        CRGB::Black, CRGB::Black, CRGB::Green,
        CRGB::Black, CRGB::Black, CRGB::Black
    };

    setButtonTypesAndColours(types, colours);
    FastLED.show();
}