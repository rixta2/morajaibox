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
        case Pink:
            // Logic for pink button
            Serial.println("Pink button action!");
            break;
        case Red:
            // Red button: Brown tiles → Red, White tiles → Brown, others unchanged
            for (int i = 0; i < 9; i++) {
                if (gridButtons[i]->type == Brown) {
                    gridButtons[i]->type = Red;
                    gridButtons[i]->colour = getButtonColor(Red);
                } else if (gridButtons[i]->type == White) {
                    gridButtons[i]->type = Brown;
                    gridButtons[i]->colour = getButtonColor(Brown);
                }
                // Other colors remain unchanged
            }
            Serial.println("Red button transformed grid: Brown→Red, White→Brown!");
            break;
        case Orange: {
            int row = btn->row;
            int col = btn->col;
            
            // Count adjacent colors (orthogonal only)
            int colorCount[10] = {0}; // Array to count each ButtonType (0-9)
            
            // Check orthogonal directions: north, south, east, west
            int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
            for (int d = 0; d < 4; d++) {
                int newRow = row + directions[d][0];
                int newCol = col + directions[d][1];
                
                Button* neighbor = getButtonAt(newRow, newCol);
                if (neighbor) {
                    colorCount[static_cast<int>(neighbor->type)]++;
                }
            }
            
            // Find plurality color (most frequent, but must be unique winner)
            int maxCount = 0;
            ButtonType pluralityColor = Orange;
            bool hasTie = false;
            
            for (int i = 0; i < 10; i++) {
                if (colorCount[i] > maxCount) {
                    maxCount = colorCount[i];
                    pluralityColor = static_cast<ButtonType>(i);
                    hasTie = false;
                } else if (colorCount[i] == maxCount && maxCount > 0) {
                    hasTie = true;
                }
            }
            
            if (maxCount > 0 && !hasTie && pluralityColor != Orange) {
                btn->type = pluralityColor;
                btn->colour = getButtonColor(pluralityColor);
                Serial.print("Orange button changed to plurality color: ");
                Serial.println(static_cast<int>(pluralityColor));
            } else {
                Serial.println("Orange button - no clear plurality, no change");
            }
            break;
        }
        case Blue: {
            // Blue button: Mimics the functionality of the center button (B2)
            Button* centerButton = getButtonAt(1, 1); // Center tile (B2)
            if (!centerButton || centerButton->type == Blue) {
                Serial.println("Blue button - center is blue or invalid, no action");
                break;
            }
            
            // Temporarily change this blue button's type to match center
            ButtonType originalType = btn->type;
            CRGB originalColor = btn->colour;
            btn->type = centerButton->type;
            btn->colour = centerButton->colour;
            
            // Execute the center button's functionality from this position
            handleButtonFunction(btn);
            
            // Restore original blue type and color
            btn->type = originalType;
            btn->colour = originalColor;
            
            Serial.print("Blue button mimicked ");
            Serial.print(centerButton->type);
            Serial.println(" behavior!");
            break;
        }
        case Violet: {
            int row = btn->row;
            int col = btn->col;
            
            // If button is in bottom row (C row), do nothing
            if (row == 2) {
                Serial.println("Violet button in bottom row - no action");
                break;
            }
            
            // Find the button below (one row down)
            int belowRow = row + 1;
            Button* belowButton = getButtonAt(belowRow, col);
            
            if (belowButton) {
                // Swap types and colors
                ButtonType tempType = btn->type;
                CRGB tempColor = btn->colour;
                
                btn->type = belowButton->type;
                btn->colour = belowButton->colour;
                
                belowButton->type = tempType;
                belowButton->colour = tempColor;
                
                Serial.println("Violet button swapped with button below!");
            }
            break;
        }
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
                        // Other colors (Pink, Red, Orange, Blue, Violet, Yellow, Brown, Green) remain unchanged
                    }
                }
            }
            
            Serial.println("White button turned black, adjacent black/white buttons toggled!");
            break;
        }
        case Black:
            // Logic for black button (if any)
            Serial.println("Black button action!");
            break;
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