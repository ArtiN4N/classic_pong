// <3 Made with raylib
#include "include/raylib.h"
#include "include/raymath.h"
// <3

#include "include/paddle.h"

#include "stdio.h"
#include "math.h"
#include "stdbool.h"

//-- SCREEN DIMENSIONS --
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
//-----------------------


//------ GAME STRUCTS ------
typedef struct { // The ball is hit by paddles towards their enemy's goalzone
    Vector2 position;
    Vector2 velocity;

    float speed;
    float radius;
} Ball;

typedef struct { // Keeps track of all held inputs to compare to relevant values
    int total_keys;
    int key_count;
    int keys_down[5];
} InputState;

typedef struct { // Stores the game state for systems to interpret and update
    Paddle paddles[2];

    Ball ball;

    InputState input_state;

    bool single_player;

    float time;
    float speedup_in;

    float reset_animation;

    bool paddle1_last_scored;
    bool in_reset_animation;

    bool winner;

    bool close;

    bool paused;
} Game;
//--------------------------


//-------------------- INITIAL GAME STATE SYSTEM --------------------
Game initial_game_state() {
    Game game;

    game.paddles[0] = create_paddle(PLAYER_ONE);
    game.paddles[1] = create_paddle(PLAYER_TWO);


    float ball_x = (float) SCREEN_WIDTH / 2.0f; // Middle of screen x-axis
    float ball_y = (float) SCREEN_HEIGHT / 2.0f; // Middle of screen y-axis
    float ball_speed = 425.0f;
    float ball_radius = 20.0f;

    game.ball = (Ball) {
        (Vector2) {
            ball_x, 
            ball_y
        },
        Vector2Normalize((Vector2) {-1, 0}), // Velocity starts at 0
        ball_speed,
        ball_radius
    };

    game.single_player = false; // Game defaults to single player being false

    game.time = 0.0f;
    game.speedup_in = 5.0f;

    game.reset_animation = 0.0f;

    game.paddle1_last_scored = true;
    game.in_reset_animation = false;

    game.winner = false;

    game.close = false;

    game.paused = true;

    return game;
}
//-------------------------------------------------------------------


//----- INITIAL INPUT STATE SYSTEM -----
InputState initial_input_state() {
    InputState input_state;

    input_state.key_count = 0;
    input_state.total_keys = 5; // We only care about 5 keys: up and down for paddle 1 and paddle 2, and a pause key

    // KEY_NULL essentially represents nothing, is used as the default key stored
    input_state.keys_down[0] = KEY_NULL;
    input_state.keys_down[1] = KEY_NULL;
    input_state.keys_down[2] = KEY_NULL;
    input_state.keys_down[3] = KEY_NULL;
    input_state.keys_down[4] = KEY_NULL;

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

    //if (IsKeyPressed(KEY_ESCAPE)) add_to_input_state(&input_state, KEY_ESCAPE);

    return input_state;
}
//--------------------------------------------------------------------------


//-------------------------------------------- INPUT HANDLE SYSTEM --------------------------------------------
void handle_input(Game* game, float dt) {
    
    for (int i = 0; i < game->input_state.total_keys; i++) {
        if (!game->winner && !game->paused) {
            
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

void speedup_ball(Game* game) {
    game->ball.speed *= 1.2;
    if (game->ball.speed > 3000.0f) game->ball.speed = 3000.0f;
    game->speedup_in = 5.0f;

}

bool detect_collision(Paddle* paddle, Ball* ball, Vector2 previous_ball_position) {
    Vector2 colliding_paddle = ball->position;

    if (ball->position.x < paddle->position.x) colliding_paddle.x = paddle->position.x; // test left edge
    else if (ball->position.x > paddle->position.x + paddle->size.x) colliding_paddle.x = paddle->position.x + paddle->size.x; // right edge

    if (ball->position.y < paddle->position.y) colliding_paddle.y = paddle->position.y; // test left edge
    else if (ball->position.y > paddle->position.y + paddle->size.y) colliding_paddle.y = paddle->position.y + paddle->size.y; // right edge

    Vector2 distanceVec = { ball->position.x - colliding_paddle.x, ball->position.y - colliding_paddle.y };
    float distance = sqrt( (distanceVec.x * distanceVec.x) + (distanceVec.y * distanceVec.y) );

    if (distance <= ball->radius) {
        
        bool on_top_or_bottom = previous_ball_position.y + ball->radius < paddle->position.y || previous_ball_position.y - ball->radius > paddle->position.y + paddle->size.y;
        bool between_paddle = previous_ball_position.x + ball->radius > paddle->position.x && previous_ball_position.x - ball->radius < paddle->position.x + paddle->size.x;
        if (between_paddle) {
            if (ball->position.y > paddle->position.y + paddle->size.y / 2.0f) {
                ball->position.y = paddle->position.y + paddle->size.y + ball->radius;
                ball->velocity.y = 1;
            } else {
                ball->position.y = paddle->position.y - ball->radius;
                ball->velocity.y = -1;
            }
        }
        else if (!between_paddle) {
            ball->position.x = previous_ball_position.x;
            if (ball->position.y < paddle->position.y + paddle->size.y * 1.0f / 3.0f) {
                ball->velocity.x *= -1;
                ball->velocity.y = -1;
            } else if (ball->position.y > paddle->position.y + paddle->size.y * 2.0f / 3.0f) {
                ball->velocity.x *= -1;
                ball->velocity.y = 1;
            } else {
                ball->velocity.x *= -1;
                ball->velocity.y = 0;
            }
        }
        return true;
    }
    return false;
}

void reset_on_score(Game* game, bool paddle1_scored) {
    float ball_x = (float) SCREEN_WIDTH / 2.0f; // Middle of screen x-axis
    float ball_y = (float) SCREEN_HEIGHT / 2.0f; // Middle of screen y-axis
    float ball_speed = 425.0f;
    float ball_radius = 20.0f;

    game->ball = (Ball) {
        (Vector2) {
            ball_x, 
            ball_y
        },
        Vector2Normalize((Vector2) {0.0f, 0.0f}), // Velocity starts at 0
        ball_speed,
        ball_radius
    };

    reset_paddle(&game->paddles[0]);
    reset_paddle(&game->paddles[1]);

    game->speedup_in = 5.0f;
    game->time = 0.0f;

    game->reset_animation = 1.0f;

    game->paddle1_last_scored = paddle1_scored;

}


//-------------------------------------------------------------------- STEP PHYSICS SYSTEM --------------------------------------------------------------------
void step_physics(Game* game, float dt) {
    if (game->speedup_in > 0.0f) game->speedup_in -= dt;
    else game->speedup_in = 0.0f;

    if (game->reset_animation > 0.0f) {
        game->reset_animation -= dt;
        game->in_reset_animation = true;
    }
    else if (game->in_reset_animation) {
        game->reset_animation = 0.0f;

        float direction = -1;
        if (game->paddle1_last_scored) direction = 1;

        game->ball.velocity.x = direction;
        game->in_reset_animation = false;
    }

    if (!game->in_reset_animation) game->time += dt;

    // NOTE: both paddles use paddle 1's height. If paddles have differing heights, this needs to be updated
    Paddle* paddle1 = &(game->paddles[0]);
    Paddle* paddle2 = &(game->paddles[1]);
    Ball* ball = &(game->ball);

    float screen_bottom = (float) SCREEN_HEIGHT; 
    float screen_top = 0.0f;

    // Keeping paddles on-screen
    update_paddle(&game->paddles[0]);
    update_paddle(&game->paddles[1]);

    // Updates balls position
    Vector2 previous_ball_position = ball->position;
    Vector2 ball_normal = Vector2Normalize(ball->velocity);

    ball->position.x += ball_normal.x * ball->speed * dt;
    ball->position.y += ball_normal.y * ball->speed * dt;

    //Keeping ball on screen
    if (ball->position.y - ball->radius < screen_top) {
        ball->position.y = screen_top + ball->radius;
        ball->velocity.y *= -1;
    }
    
    if (ball->position.y + ball->radius > screen_bottom) {
        ball->position.y = screen_bottom - ball->radius;
        ball->velocity.y *= -1;
    }

    bool collision = false;

    collision = detect_collision(paddle1, ball, previous_ball_position);
    if (!collision) collision = detect_collision(paddle2, ball, previous_ball_position);

    if (collision && game->speedup_in == 0.0f) speedup_ball(game);

    if (ball->position.x - ball->radius > SCREEN_WIDTH) {
        score(&game->paddles[0]);
        reset_on_score(game, true);
    }
    else if (ball->position.x + ball->radius < 0) {
        score(&game->paddles[1]);
        reset_on_score(game, false);
    }

    if (game->paddles[0].score == 10 || game->paddles[1].score == 10) game->winner = true;

    
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------


// DRAW GAME SYSTEM
void draw_game(Game* game) {
    BeginDrawing();

        ClearBackground(BLACK);

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

        if (game->winner) {
            float win_text_x = (SCREEN_WIDTH - MeasureText("Player 1 Wins!!!", 80)) / 2;
            float win_text_y = (SCREEN_HEIGHT - 80.0f) / 2.0f - 200.0f;
            DrawRectangle(win_text_x - 50, win_text_y - 50, MeasureText("Player 1 Wins!!!", 80) + 100, 80 + 100, BLACK);
            if (game->paddle1_last_scored) DrawText("Player 1 Wins!!!", win_text_x, win_text_y, 80, WHITE);
            else DrawText("Player 2 Wins!!!", win_text_x, win_text_y, 80, WHITE);

            float reset_text_x = (SCREEN_WIDTH - MeasureText("Press Escape to Exit", 50)) / 2;
            float reset_text_y = (SCREEN_HEIGHT - 50.0f) / 2.0f;
            DrawRectangle(reset_text_x - 50, reset_text_y - 50, MeasureText("Press Escape to Exit", 50) + 100, 50 + 100, BLACK);
            DrawText("Press Escape to Exit", reset_text_x, reset_text_y, 50, WHITE);
        }


    EndDrawing();
}
//

void draw_menu() {
    BeginDrawing();
        ClearBackground(BLACK);
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

        


    EndDrawing();
}


int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT); // Turns on v-sync

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "classic_pong");

    SetExitKey(KEY_NULL); // Disables exiting with escape key

    Game game = initial_game_state();

    while (!game.close && !WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_BACKSPACE)) game.close = true;

        if (IsKeyPressed(KEY_ESCAPE) && (game.paused || game.winner)) {
            game.paused = false;
            if (game.winner) {
                game.winner = false;
                game = initial_game_state();
            }
        }

        if (!game.paused) {
            game.input_state = capture_input();
            handle_input(&game, dt);

            if (!game.winner) step_physics(&game, dt);

            draw_game(&game);
        } else draw_menu(&game);

        
    }

    CloseWindow();

    return 0;
}