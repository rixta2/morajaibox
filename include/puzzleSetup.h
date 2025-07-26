#pragma once

#include "config.h"
#include "buttonHandler.h"

#include <FastLED.h>

// Puzzle definition structure
struct PuzzleDefinition {
    ButtonType layout[9];     // 3x3 grid layout
    ButtonType solveColor;    // Color needed in all 4 corners to win
    const char* name;         // Optional name for the puzzle
};

// Puzzle layout selection - change this value to select different layouts
extern int selectedPuzzleLayout;
extern const PuzzleDefinition puzzles[];
extern const int totalPuzzles;
extern bool useWebDeployedPuzzle;
extern ButtonType webDeployedSolveColor;
extern ButtonType webDeployedLayout[9];

void initiatePuzzle();
ButtonType getCurrentSolveColor();
bool checkWinCondition();
void resetPuzzle();
void celebrateSolve();
void handleCornerButtonPress(int cornerIndex);
void updateCornerLEDs();
bool isEffectivelyColor(Button* btn, ButtonType targetColor);