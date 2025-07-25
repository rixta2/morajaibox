# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a PlatformIO-based ESP32 puzzle box project that controls a 3x3 grid of buttons with corresponding LEDs and 4 corner LEDs. The system implements a state-machine-driven puzzle game where button presses trigger different behaviors based on their color/type.

## Development Commands

### Build and Upload
```bash
pio run                    # Build the project
pio run -t upload         # Build and upload to ESP32
pio run -t monitor        # Open serial monitor (115200 baud)
pio run -t upload -t monitor  # Upload and monitor in one command
```

### Testing
```bash
pio test                  # Run unit tests (currently no tests implemented)
```

## Architecture

### Hardware Configuration
- **Target**: ESP32 Development Board
- **LED Controller**: FastLED library with WS2812B strips
- **Grid Layout**: 3x3 button/LED matrix (pins 22, 27, 2)
- **Corner LEDs**: 4 individual LEDs (pins 12, 15, 26, 14)
- **Button Pins**: 9 buttons mapped to GPIO pins 5, 17, 16, 21, 19, 18, 25, 32, 33

### State Machine
The puzzle operates through `puzzleState` enum:
- `INITIALISE`: Sets up initial puzzle configuration
- `PLAYING`: Main game loop handling button inputs
- `SOLVED`: Puzzle completion state (not fully implemented)
- `RESET`: Reset state (not fully implemented)

### Core Components

#### Button System (`buttonHandler.h/cpp`)
- Each button has a `Button` struct containing pin, position, state, color, and type
- Button types: Brown, Red, Blue, Yellow, White, Black
- `ButtonHandler()` processes all 9 buttons each loop cycle
- `handleButtonFunction()` implements type-specific button logic

#### LED Management (`config.h`, `LEDcontroller.h`)
- `grid[3][3]` array for main button LEDs
- `corner[4][1]` array for corner LEDs
- LED states directly controlled by button color properties

#### Puzzle Logic (`puzzleSetup.h/cpp`)
- `initiatePuzzle()` sets initial button types and colors
- Currently implements a simple green button cycling mechanism

### Code Organization
- **Headers**: `include/` directory contains all header files
- **Implementation**: `src/` directory contains .cpp files
- **Configuration**: Shared constants and global variables in `config.h/cpp`
- **Main Loop**: `src/main.cpp` contains setup() and state machine loop()

## Key Implementation Notes

- All button states are stored in global `lastButtonState[13]` array for debouncing
- LED updates happen immediately in `ButtonHandler()` via `FastLED.show()`
- Button press detection uses INPUT_PULLUP configuration (LOW = pressed)
- Brown buttons implement a cycling behavior that cycles all colors in the row to the right
- Most puzzle logic beyond green buttons is not yet implemented