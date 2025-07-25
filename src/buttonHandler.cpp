#include "buttonHandler.h"
#include "config.h"
#include <FastLED.h>

Button GA1 = {5, 0, 2, false, false, 0, CRGB::Black, Brown};
Button GA2 = {17, 0, 1, false, false, 0, CRGB::Black, Red};
Button GA3 = {16, 0, 0, false, false, 0, CRGB::Black, Blue};
Button GB1 = {21, 1, 2, false, false, 0, CRGB::Black, Yellow};
Button GB2 = {19, 1, 1, false, false, 0, CRGB::Black, White};
Button GB3 = {18, 1, 0, false, false, 0, CRGB::Black, Brown};
Button GC1 = {25, 2, 2, false, false, 0, CRGB::Black, Red};
Button GC2 = {32, 2, 1, false, false, 0, CRGB::Black, Blue};
Button GC3 = {33, 2, 0, false, false, 0, CRGB::Black, Yellow};

Button* gridButtons[9] = {&GA1, &GA2, &GA3, &GB1, &GB2, &GB3, &GC1, &GC2, &GC3};

// Helper function to get button at specific row/col
Button* getButtonAt(int row, int col) {
    for (int i = 0; i < 9; i++) {
        if (gridButtons[i]->row == row && gridButtons[i]->col == col) {
            return gridButtons[i];
        }
    }
    return nullptr;
}

// Helper function for flood-fill to find connected tiles of same color
void floodFillConnected(int row, int col, ButtonType targetType, ButtonType newType, bool visited[3][3]) {
    // Check bounds
    if (row < 0 || row > 2 || col < 0 || col > 2) return;
    if (visited[row][col]) return;
    
    Button* btn = getButtonAt(row, col);
    if (!btn || btn->type != targetType) return;
    
    // Mark as visited and change type
    visited[row][col] = true;
    btn->type = newType;
    btn->colour = getButtonColor(newType);
    
    // Recursively check orthogonal neighbors
    floodFillConnected(row - 1, col, targetType, newType, visited); // Up
    floodFillConnected(row + 1, col, targetType, newType, visited); // Down
    floodFillConnected(row, col - 1, targetType, newType, visited); // Left
    floodFillConnected(row, col + 1, targetType, newType, visited); // Right
}

// Helper function to convert black tiles adjacent to newly blacked tiles to white
void convertAdjacentBlackToWhite(bool blackedTiles[3][3]) {
    // Create list of tiles to convert (to avoid modifying during iteration)
    bool tilesToConvert[3][3] = {false};
    
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (blackedTiles[row][col]) {
                // Check orthogonal neighbors
                int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                for (int d = 0; d < 4; d++) {
                    int newRow = row + directions[d][0];
                    int newCol = col + directions[d][1];
                    
                    // Check bounds
                    if (newRow >= 0 && newRow < 3 && newCol >= 0 && newCol < 3) {
                        // Don't convert tiles that were just blacked in Phase 1
                        if (!blackedTiles[newRow][newCol]) {
                            Button* neighbor = getButtonAt(newRow, newCol);
                            if (neighbor && neighbor->type == Black) {
                                tilesToConvert[newRow][newCol] = true;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Convert marked tiles to white
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (tilesToConvert[row][col]) {
                Button* btn = getButtonAt(row, col);
                if (btn) {
                    btn->type = White;
                    btn->colour = getButtonColor(White);
                }
            }
        }
    }
}

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

void setButtonTypes(ButtonType types[9]) {
    for (int i = 0; i < 9; i++) {
        gridButtons[i]->type = types[i];
        gridButtons[i]->colour = getButtonColor(types[i]);
    }
}

void handleButtonFunction(Button* btn) {
    switch (btn->type) {
        case Brown: {
            int row = btn->row;
            
            // Store the current types of all buttons in this row
            ButtonType rowTypes[3];
            for (int col = 0; col < 3; col++) {
                for (int i = 0; i < 9; i++) {
                    if (gridButtons[i]->row == row && gridButtons[i]->col == col) {
                        rowTypes[col] = gridButtons[i]->type;
                        break;
                    }
                }
            }
            
            // Cycle all colors in the row to the right (wrap around)
            for (int col = 0; col < 3; col++) {
                int nextCol = (col + 1) % 3; // Next column (wrapping)
                for (int i = 0; i < 9; i++) {
                    if (gridButtons[i]->row == row && gridButtons[i]->col == col) {
                        gridButtons[i]->type = rowTypes[nextCol];
                        gridButtons[i]->colour = getButtonColor(rowTypes[nextCol]);
                        break;
                    }
                }
            }
            Serial.println("Brown button cycled row!");
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
        case Yellow: {
            int row = btn->row;
            int col = btn->col;
            
            // If button is in top row (A row), do nothing
            if (row == 0) {
                Serial.println("Yellow button in top row - no action");
                break;
            }
            
            // Find the button above (one row up)
            int aboveRow = row - 1;
            Button* aboveButton = nullptr;
            for (int i = 0; i < 9; i++) {
                if (gridButtons[i]->row == aboveRow && gridButtons[i]->col == col) {
                    aboveButton = gridButtons[i];
                    break;
                }
            }
            
            if (aboveButton) {
                // Swap types and colors
                ButtonType tempType = btn->type;
                CRGB tempColor = btn->colour;
                
                btn->type = aboveButton->type;
                btn->colour = aboveButton->colour;
                
                aboveButton->type = tempType;
                aboveButton->colour = tempColor;
                
                Serial.println("Yellow button swapped with button above!");
            }
            break;
        }
        case White: {
            int row = btn->row;
            int col = btn->col;
            
            // Turn the pressed white button black
            btn->type = Black;
            btn->colour = getButtonColor(Black);
            
            // Toggle adjacent buttons (black ↔ white, other colors unchanged)
            int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
            for (int d = 0; d < 4; d++) {
                int newRow = row + directions[d][0];
                int newCol = col + directions[d][1];
                
                // Check bounds
                if (newRow >= 0 && newRow < 3 && newCol >= 0 && newCol < 3) {
                    Button* neighbor = getButtonAt(newRow, newCol);
                    if (neighbor) {
                        if (neighbor->type == White) {
                            neighbor->type = Black;
                            neighbor->colour = getButtonColor(Black);
                        } else if (neighbor->type == Black) {
                            neighbor->type = White;
                            neighbor->colour = getButtonColor(White);
                        }
                        // Other colors (Brown, Red, Blue, Yellow, Green) remain unchanged
                    }
                }
            }
            
            Serial.println("White button turned black, adjacent black/white buttons toggled!");
            break;
        }
        case Black:
            // Logic for black button (if any)
            break;
        case Green: {
            int row = btn->row;
            int col = btn->col;
            
            // Calculate opposite position
            int oppositeRow = 2 - row; // A(0) <-> C(2), B(1) stays B(1)
            int oppositeCol = 2 - col; // 1(0) <-> 3(2), 2(1) stays 2(1)
            
            // If button is in center (B2), do nothing
            if (row == 1 && col == 1) {
                Serial.println("Green button in center - no action");
                break;
            }
            
            // Find the opposite button
            Button* oppositeButton = nullptr;
            for (int i = 0; i < 9; i++) {
                if (gridButtons[i]->row == oppositeRow && gridButtons[i]->col == oppositeCol) {
                    oppositeButton = gridButtons[i];
                    break;
                }
            }
            
            if (oppositeButton) {
                // Swap types and colors
                ButtonType tempType = btn->type;
                CRGB tempColor = btn->colour;
                
                btn->type = oppositeButton->type;
                btn->colour = oppositeButton->colour;
                
                oppositeButton->type = tempType;
                oppositeButton->colour = tempColor;
                
                Serial.println("Green button swapped with opposite!");
            }
            break;
        }
        default:
            break;
    }
}