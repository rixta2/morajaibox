#pragma once

enum puzzleState {
    INITIALISE,
    PLAYING,
    SOLVED,
    RESET
};

extern volatile puzzleState currentState;