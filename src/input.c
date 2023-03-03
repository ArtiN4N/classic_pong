#include "include/raylib.h"
#include "include/input.h"

//----- INITIAL INPUT STATE SYSTEM -----
InputState initial_input_state() {
    InputState input_state;

    input_state.key_count = 0;
    input_state.total_keys = 8; // We care about 8 keys: up and down for paddle 1 and paddle 2, an exit key, a return to menu key, and 2 start game keys

    // KEY_NULL essentially represents nothing, is used as the default key stored
    input_state.keys_down[0] = KEY_NULL;
    input_state.keys_down[1] = KEY_NULL;
    input_state.keys_down[2] = KEY_NULL;
    input_state.keys_down[3] = KEY_NULL;
    input_state.keys_down[4] = KEY_NULL;
    input_state.keys_down[5] = KEY_NULL;
    input_state.keys_down[6] = KEY_NULL;
    input_state.keys_down[7] = KEY_NULL;

    return input_state;
}
//--------------------------------------


//----------------- ADD KEY TO INPUT STATE -----------------
void add_to_input_state(InputState* input_state, int key) {
    input_state->keys_down[input_state->key_count] = key; // Add the key to the current available index
    input_state->key_count++; // Update index counter
}
//----------------------------------------------------------


//-------------------------- INPUT CAPTURE SYSTEM --------------------------
InputState capture_input() {
    InputState input_state = initial_input_state();

    if (IsKeyDown(KEY_S)) add_to_input_state(&input_state, KEY_S); // paddle1 down
    if (IsKeyDown(KEY_W)) add_to_input_state(&input_state, KEY_W); // paddle1 up

    if (IsKeyDown(KEY_K)) add_to_input_state(&input_state, KEY_K); // paddle2 down
    if (IsKeyDown(KEY_I)) add_to_input_state(&input_state, KEY_I); // paddle2 up

    if (IsKeyDown(KEY_ONE)) add_to_input_state(&input_state, KEY_ONE); // singleplayer
    if (IsKeyDown(KEY_TWO)) add_to_input_state(&input_state, KEY_TWO); // multiplayer

    if (IsKeyDown(KEY_BACKSPACE)) add_to_input_state(&input_state, KEY_BACKSPACE); // exit
    if (IsKeyPressed(KEY_ESCAPE)) add_to_input_state(&input_state, KEY_ESCAPE); // pause

    return input_state;
}
//--------------------------------------------------------------------------