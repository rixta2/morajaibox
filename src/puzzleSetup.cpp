#include "puzzleSetup.h"
#include "buttonHandler.h"

// Change this value to select different puzzle layouts (0-4)
int selectedPuzzleLayout = 2;
bool useWebDeployedPuzzle = false;
ButtonType webDeployedSolveColor = Green;
ButtonType webDeployedLayout[9] = {Green, Green, Green, Green, Green, Green, Green, Green, Green};

// Unified puzzle definitions - layout and solve color together
const PuzzleDefinition puzzles[] = {
    
    {
        {Green, Black, Yellow,
         Green, Yellow, Green,
         Yellow, Black, Green},
        Green,
        "Green Box 1"
    },

    {
        {Brown, Green, Blue,
         Blue, Blue, Blue,
         Violet, Black, Black},
        Blue,
        "Blue Box 1"
    },

    {
        {Orange, Brown, Orange,
         Orange, Red, Orange,
         Orange, Brown, Orange},
        Red,
        "Red Box 1"
    },
    

};

const int totalPuzzles = sizeof(puzzles) / sizeof(puzzles[0]);

void initiatePuzzle() {
    if (useWebDeployedPuzzle) {
        // Re-apply the web-deployed puzzle layout (for resets)
        setButtonTypes(webDeployedLayout);
    } else {
        // Use hardcoded puzzles
        // Ensure selectedPuzzleLayout is within valid range
        if (selectedPuzzleLayout < 0 || selectedPuzzleLayout >= totalPuzzles) {
            selectedPuzzleLayout = 0;
        }
        
        setButtonTypes((ButtonType*)puzzles[selectedPuzzleLayout].layout);
    }
    
    // Turn off all corner LEDs initially
    for (int i = 0; i < 4; i++) {
        corner[i][0] = CRGB::Black;
    }
    
    FastLED.show();
}

ButtonType getCurrentSolveColor() {
    if (useWebDeployedPuzzle) {
        return webDeployedSolveColor;
    } else {
        return puzzles[selectedPuzzleLayout].solveColor;
    }
}

bool checkWinCondition() {
    ButtonType solveColor = getCurrentSolveColor();
    
    // Check if corner grid positions A1, A3, C1, C3 effectively match solve color
    // A1 = gridButtons[0], A3 = gridButtons[2], C1 = gridButtons[6], C3 = gridButtons[8]
    return (isEffectivelyColor(gridButtons[0], solveColor) &&  // A1 (top-left)
            isEffectivelyColor(gridButtons[2], solveColor) &&  // A3 (top-right)
            isEffectivelyColor(gridButtons[6], solveColor) &&  // C1 (bottom-left)
            isEffectivelyColor(gridButtons[8], solveColor));   // C3 (bottom-right)
}

void resetPuzzle() {
    initiatePuzzle();
}

void celebrateSolve() {
    ButtonType solveColor = getCurrentSolveColor();
    CRGB celebrationColor = getButtonColor(solveColor);
    
    // Flash all LEDs the solve color 5 times
    for (int flash = 0; flash < 5; flash++) {
        // Turn all LEDs to solve color
        for (int strip = 0; strip < 3; strip++) {
            for (int led = 0; led < 3; led++) {
                grid[strip][led] = celebrationColor;
            }
        }
        for (int i = 0; i < 4; i++) {
            corner[i][0] = celebrationColor;
        }
        FastLED.show();
        delay(300);
        
        // Turn all LEDs off
        for (int strip = 0; strip < 3; strip++) {
            for (int led = 0; led < 3; led++) {
                grid[strip][led] = CRGB::Black;
            }
        }
        for (int i = 0; i < 4; i++) {
            corner[i][0] = CRGB::Black;
        }
        FastLED.show();
        delay(300);
    }
    
    // Move to next puzzle layout  
    useWebDeployedPuzzle = false; // Switch back to hardcoded puzzles
    selectedPuzzleLayout = (selectedPuzzleLayout + 1) % totalPuzzles;
    initiatePuzzle();
}

void handleCornerButtonPress(int cornerIndex) {
    ButtonType solveColor = getCurrentSolveColor();
    ButtonType cornerGridColor;
    
    // Map corner button index to grid button index
    // 0=TL(A1), 1=TR(A3), 2=BL(C1), 3=BR(C3)
    int gridIndex[] = {0, 2, 6, 8}; // A1, A3, C1, C3
    
    // Map corner button index to LED array index
    // Button indices: 0=TL, 1=TR, 2=BL, 3=BR
    // But LEDs are wired differently, so we need to map correctly:
    // BR button(3) -> BL LED(2), BL button(2) -> BR LED(3)
    // TR button(1) -> TL LED(0), TL button(0) -> TR LED(1)
    int ledIndex[] = {1, 0, 3, 2}; // TL->TR, TR->TL, BL->BR, BR->BL
    
    Button* cornerButton = gridButtons[gridIndex[cornerIndex]];
    
    if (isEffectivelyColor(cornerButton, solveColor)) {
        // Correct color - light up corner LED with solve color
        corner[ledIndex[cornerIndex]][0] = getButtonColor(solveColor);
        FastLED.show();
        
        // Check if all corners are now correct (win condition)
        if (checkWinCondition()) {
            // Check if all corner LEDs are lit with solve color
            bool allCornerLEDsCorrect = true;
            CRGB solveColorRGB = getButtonColor(solveColor);
            
            for (int i = 0; i < 4; i++) {
                if (corner[i][0] != solveColorRGB) {
                    allCornerLEDsCorrect = false;
                    break;
                }
            }
            
            if (allCornerLEDsCorrect) {
                celebrateSolve();
            }
        }
    } else {
        // Wrong color - reset puzzle
        resetPuzzle();
    }
}

void updateCornerLEDs() {
    ButtonType solveColor = getCurrentSolveColor();
    CRGB solveColorRGB = getButtonColor(solveColor);
    
    // Grid positions for corners: A1, A3, C1, C3
    int gridIndex[] = {0, 2, 6, 8}; // A1, A3, C1, C3
    // LED mapping: TL->TR, TR->TL, BL->BR, BR->BL
    int ledIndex[] = {1, 0, 3, 2};
    
    // Check each corner and update LED accordingly
    for (int i = 0; i < 4; i++) {
        if (isEffectivelyColor(gridButtons[gridIndex[i]], solveColor)) {
            // Corner has correct color - LED should be lit if it was previously activated
            // Only keep LED on if it was already on (player pressed the button)
            // If LED is currently off, don't turn it on automatically
        } else {
            // Corner has wrong color - turn off LED
            corner[ledIndex[i]][0] = CRGB::Black;
        }
    }
    
    FastLED.show();
}

bool isEffectivelyColor(Button* btn, ButtonType targetColor) {
    // Check if a button effectively represents the target color
    // For Blue: check if it appears blue (has blue color), regardless of internal type
    // For other colors: check if the type matches
    
    if (targetColor == Blue) {
        // For Blue solve color, check if button appears blue (has blue color)
        return btn->colour == getButtonColor(Blue);
    } else {
        // For non-Blue solve colors, check actual type
        return btn->type == targetColor;
    }
}