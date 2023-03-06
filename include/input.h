#ifndef INPUT_H
#define INPUT_H

// Keeps track of all relevant inputs to update game state.
typedef struct { 
    int total_keys;
    int key_count;
    int keys_down[4];
} InputState;

// Creates a default input state. Returns an InputState struct.
InputState initial_input_state();

void add_to_input_state(InputState* input_state, int key);

// Polls for current inputs.
InputState capture_input();

#endif