// <3 Made with raylib
#include "../include/raylib.h"

#include "../include/game.h"


int main(void) {

    SetConfigFlags(FLAG_VSYNC_HINT); // Turns on v-sync

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "classic_pong");

    SetExitKey(KEY_NULL); // Disables exiting with escape key

    Game game = initial_game_state();


    //------------------------------------------------------


    while (!game.close && !WindowShouldClose()) { // Main game loop
        
        float dt = GetFrameTime();

        game.input_state = capture_input();


        //------------------------------------------------------


        handle_input(&game, dt);

        step_physics(&game, dt);

        draw_game(game);

    }


    //------------------------------------------------------


    CloseWindow();

    return 0;

}
