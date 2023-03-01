// <3 Made with raylib
#include "include/raylib.h"
#include "include/raymath.h"
// <3

#include "include/paddle.h"
#include "include/ball.h"

#include "stdio.h"
#include "math.h"
#include "stdbool.h"

//-- SCREEN DIMENSIONS --
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
//-----------------------


//------ GAME STRUCTS ------

typedef struct { // Keeps track of all held inputs to compare to relevant values
    int total_keys;
    int key_count;
    int keys_down[6];
} InputState;

typedef enum ScreenEvent { MENU = 0, PLAY, WIN } ScreenEvent;

typedef struct { // Stores the game state for systems to interpret and update
    Paddle paddles[2];

    Ball ball;

    InputState input_state;

    bool single_player;

    float time;
    float speedup_in;

    float reset_animation;

    Player paddle_last_scored;
    bool in_reset_animation;

    bool winner;

    bool close;

    bool paused;

    ScreenEvent screen_event;
} Game;
//--------------------------


//-------------------- INITIAL GAME STATE SYSTEM --------------------
Game initial_game_state() {
    Game game;

    game.paddles[0] = create_paddle(PLAYER_ONE);
    game.paddles[1] = create_paddle(PLAYER_TWO);

    game.ball = create_ball();

    game.single_player = false; // Game defaults to single player being false

    game.time = 0.0f;
    game.speedup_in = 5.0f;

    game.reset_animation = 0.0f;

    game.paddle_last_scored = PLAYER_ONE;
    game.in_reset_animation = false;

    game.winner = false;

    game.close = false;

    game.paused = true;

    game.screen_event = MENU;

    return game;
}
//-------------------------------------------------------------------


//----- INITIAL INPUT STATE SYSTEM -----
InputState initial_input_state() {
    InputState input_state;

    input_state.key_count = 0;
    input_state.total_keys = 6; // We only care about 5 keys: up and down for paddle 1 and paddle 2, and a pause key

    // KEY_NULL essentially represents nothing, is used as the default key stored
    input_state.keys_down[0] = KEY_NULL;
    input_state.keys_down[1] = KEY_NULL;
    input_state.keys_down[2] = KEY_NULL;
    input_state.keys_down[3] = KEY_NULL;
    input_state.keys_down[4] = KEY_NULL;
    input_state.keys_down[5] = KEY_NULL;

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

    if (IsKeyDown(KEY_S)) add_to_input_state(&input_state, KEY_S);
    if (IsKeyDown(KEY_W)) add_to_input_state(&input_state, KEY_W);
    if (IsKeyDown(KEY_K)) add_to_input_state(&input_state, KEY_K);
    if (IsKeyDown(KEY_I)) add_to_input_state(&input_state, KEY_I);
    if (IsKeyDown(KEY_BACKSPACE)) add_to_input_state(&input_state, KEY_BACKSPACE); // exit
    if (IsKeyPressed(KEY_ESCAPE)) add_to_input_state(&input_state, KEY_ESCAPE); // pause

    return input_state;
}
//--------------------------------------------------------------------------


//-------------------------------------------- INPUT HANDLE SYSTEM --------------------------------------------
void handle_input(Game* game, float dt) {
    if (IsKeyDown(KEY_BACKSPACE)) {
        game->close = true;
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (game->screen_event == MENU) {
            game->screen_event = PLAY;
            return;
        }
        if (game->screen_event == WIN) {
            game->screen_event = MENU;
            *game = initial_game_state();
            return;
        }

        
    }

    


    for (int i = 0; i < game->input_state.total_keys; i++) {
        
        if (game->screen_event == PLAY) {
            // Handles paddle 1 movement
            if (game->input_state.keys_down[i] == KEY_S) move_paddle(&game->paddles[0],  1, dt);
            if (game->input_state.keys_down[i] == KEY_W) move_paddle(&game->paddles[0], -1, dt);

            // If there is a second player, allow input to determine paddle 2 movement
            if (!game->single_player) {
                if (game->input_state.keys_down[i] == KEY_K) move_paddle(&game->paddles[1],  1, dt);
                if (game->input_state.keys_down[i] == KEY_I) move_paddle(&game->paddles[1], -1, dt);
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------------------

void reset_game_on_score(Game* game, Paddle paddle) {
    
    reset_ball(&game->ball);

    reset_paddle(&game->paddles[0]);
    reset_paddle(&game->paddles[1]);

    game->time = 0.0f;
    game->reset_animation = 1.0f;

    game->paddle_last_scored = paddle.player_number;

}


//-------------------------------------------------------------------- STEP PHYSICS SYSTEM --------------------------------------------------------------------
void step_physics(Game* game, float dt) {

    if (game->screen_event != PLAY) return;

    if (game->reset_animation > 0.0f) {
        game->reset_animation -= dt;
        game->in_reset_animation = true;
    }
    else if (game->in_reset_animation) {
        game->reset_animation = 0.0f;

        float direction = -1;
        if (game->paddle_last_scored == PLAYER_ONE) direction = 1;

        game->ball.direction.x = direction;
        game->in_reset_animation = false;
    }

    if (!game->in_reset_animation) game->time += dt;

    // NOTE: both paddles use paddle 1's height. If paddles have differing heights, this needs to be updated
    
    // Keeping paddles on-screen
    update_paddle(&game->paddles[0]);
    update_paddle(&game->paddles[1]);

    //
    update_ball(&game->ball, game->paddles, dt);

    Paddle* scored = check_score(game->ball, game->paddles);

    if (scored != NULL) {
        score(scored);
        reset_game_on_score(game, *scored);
    }


    if (game->paddles[0].score == 1 || game->paddles[1].score == 1) {
        game->screen_event = WIN;
        printf("%d\n", game->paddles[0].score);
    }

    
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------


// DRAW GAME SYSTEM
void draw_play(Game* game) {

    // Draw the halfway marker
    int markers = 10;
    float marker_length = (float) SCREEN_HEIGHT / 21.0;
    for (int i = 1; i < markers * 2 - 1; i += 2) {
        DrawRectangle((float) SCREEN_WIDTH / 2.0f - 2.0f, i * marker_length, 4.0f, marker_length, WHITE);
    }

    draw_paddle(game->paddles[0]);
    draw_paddle(game->paddles[1]);

    int ball_opac = 255;
    if (game->reset_animation > 0.0f) {
        int opac_frame = (int) (game->reset_animation * 10.0f);
        if (opac_frame % 2 == 0) ball_opac = (int) 255 / 2.0f;
    } 

    DrawCircleV(game->ball.position, game->ball.radius, (Color) {255, 255, 255, ball_opac}); // Draw the ball

    draw_paddle_score(game->paddles[0].player_number, game->paddles[0].score);
    draw_paddle_score(game->paddles[1].player_number, game->paddles[1].score);


    float timer_factor = game->time / 60.0f;
    if (timer_factor >= 1.0f) timer_factor = 1.0f;

    float r_offset = 25 * timer_factor;
    float g_offset = 214 * timer_factor;
    float b_offset = 200 * timer_factor;
    Color time_color = { 255 - r_offset, 255 - g_offset, 255 - b_offset, 255 };

    DrawText(TextFormat("%.1f", game->time), (SCREEN_WIDTH - MeasureText(TextFormat("%.1f", game->time), 40)) / 2, SCREEN_HEIGHT - 70.0f, 40, time_color);



}

void draw_win(Game* game) {

    float win_text_y = (SCREEN_HEIGHT - 70.0f) / 2.0f - 200.0f;


    if (game->paddle_last_scored == PLAYER_ONE) {
        const char* win_text = "Player 1 Wins";
        float win_text_x = (SCREEN_WIDTH - MeasureText(win_text, 70)) / 2;
        DrawText(win_text, win_text_x, win_text_y, 70, BLUE);
    } else {
        const char* win_text = "Player 2 Wins";
        float win_text_x = (SCREEN_WIDTH - MeasureText(win_text, 70)) / 2;
        DrawText(win_text, win_text_x, win_text_y, 70, PURPLE);        
    }

    float score_text_y = win_text_y + 100.0f;

    const char* score_dash = "-";
    const char* score_player_one = TextFormat("%d", game->paddles[0].score);
    const char* score_player_two = TextFormat("%d", game->paddles[1].score);

    float dash_x = (SCREEN_WIDTH - MeasureText(score_dash, 70)) / 2;
    float player_one_x = dash_x - 25.0f - MeasureText(score_player_one, 70);
    float player_two_x = dash_x + 25.0f + MeasureText(score_dash, 70);

    DrawText(score_dash, dash_x, score_text_y, 70, WHITE);
    DrawText(score_player_one, player_one_x, score_text_y, 70, BLUE);
    DrawText(score_player_two, player_two_x, score_text_y, 70, PURPLE);

    float reset_text_x = (SCREEN_WIDTH - MeasureText("Press Escape to Return to Menu", 40)) / 2;
    float reset_text_y = (SCREEN_HEIGHT - 40.0f) / 2.0f + 100.0f;
    DrawText("Press Escape to Return to Menu", reset_text_x, reset_text_y, 40, WHITE);

}

void draw_menu() {
    float title_text_x = (SCREEN_WIDTH - MeasureText("classic_pong", 80)) / 2;
    float title_text_y = (SCREEN_HEIGHT - 80.0f) / 2.0f - 200.0f;
    DrawText("classic_pong", title_text_x, title_text_y, 80, WHITE);

    float play_text_x = (SCREEN_WIDTH - MeasureText("Press ESCAPE to play", 30)) / 2;
    float play_text_y = (SCREEN_HEIGHT - 30.0f) / 2.0f;
    DrawText("Press ESCAPE to play", play_text_x, play_text_y, 30, GREEN);

    float paddle1_text_x = (8.0f * SCREEN_WIDTH / 20.0f) - MeasureText("Player 1 uses keys w and s", 20) / 2.0f;
    float paddle1_text_y = play_text_y + 50.0f;
    DrawText("Player 1 uses keys w and s", paddle1_text_x, paddle1_text_y, 20, BLUE);

    float paddle2_text_x = (12.0f * SCREEN_WIDTH / 20.0f) - MeasureText("Player 2 uses keys i and k", 20) / 2.0f;
    float paddle2_text_y = play_text_y + 50.0f;
    DrawText("Player 2 uses keys i and k", paddle2_text_x, paddle2_text_y, 20, PURPLE);

    float exit_text_x = (SCREEN_WIDTH - MeasureText("Press BACKSPACE to exit", 30)) / 2;
    float exit_text_y = play_text_y + 150.0f;
    DrawText("Press BACKSPACE to exit", exit_text_x, exit_text_y, 30, RED);
    
}

void draw_game(Game* game) {
    BeginDrawing();
        ClearBackground(BLACK);
        if (game->screen_event == MENU) {
            draw_menu();
        } else if (game->screen_event == PLAY) {
            draw_play(game);
        } else if (game->screen_event == WIN) {
            draw_win(game);
        }
    EndDrawing();
}

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT); // Turns on v-sync

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "classic_pong");

    SetExitKey(KEY_NULL); // Disables exiting with escape key

    Game game = initial_game_state();

    while (!game.close && !WindowShouldClose()) {
        float dt = GetFrameTime();

        game.input_state = capture_input();

        handle_input(&game, dt);

        step_physics(&game, dt);

        draw_game(&game);
    }

    CloseWindow();

    return 0;
}