#ifndef INPUT_H
#define INPUT_H

#include "raylib.h"

typedef struct { // Keeps track of all held inputs to compare to relevant values
    int total_keys;
    int key_count;
    int keys_down[8];
} InputState;

InputState initial_input_state();

void add_to_input_state(InputState* input_state, int key);

InputState capture_input();

#endif