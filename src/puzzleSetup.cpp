#include "puzzleSetup.h"
#include "buttonHandler.h"

void initiatePuzzle() {
    ButtonType types[9] = {
        White, Brown, Green,
        Brown, Black, Black,
        Green, Black, White
    };

    setButtonTypes(types);
    FastLED.show();
}