#include "../include/raylib.h"

#include "../include/game.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>



Game initial_game_state() {

    Game game;

    const float animation_length = 1.0f;

    const bool animation_loop = false;


    //--------------------------------------------------------------------------


    game.paddles[0] = create_paddle(PLAYER_ONE);
    game.paddles[1] = create_paddle(PLAYER_TWO);

    game.ball = create_ball();

    game.single_player = false; // Game defaults to single player being false

    game.time = 0.0f;
    game.draw_dt = 0.0f;

    game.reset_animation_timer = create_timer(animation_length, animation_loop);
    pause_unpause_timer(&game.reset_animation_timer);


    game.paddle_last_scored = PLAYER_ONE;

    game.close = false;

    game.screen_event = MENU;


    //--------------------------------------------------------------------------


    return game;

}


void handle_input(Game* game, float dt) {

    if (IsKeyDown(KEY_BACKSPACE)) { // Pressing backspace exits the game
        game->close = true;
        return;
    }

    if (game->screen_event == WIN && IsKeyDown(KEY_ESCAPE)) {  // Escape returns to menu on win screen
        game->screen_event = MENU;
        *game = initial_game_state();
        return;
    }

    if (game->screen_event == MENU) { // Keys one and two are used to start the game when on menu screen
        if (IsKeyDown(KEY_ONE)) {
            game->screen_event = PLAY;
            game->single_player = true;
        } else if (IsKeyDown(KEY_TWO)) {
            game->screen_event = PLAY;
            game->single_player = false;
        }
        return;
    }


    //---------------------------------------------------------------------------------------------------------


    for (int i = 0; i < game->input_state.total_keys; i++) {
        
         // Handles paddle 1 movement

        if (game->input_state.keys_down[i] == KEY_S) move_paddle(&game->paddles[0],  1, dt);
        if (game->input_state.keys_down[i] == KEY_W) move_paddle(&game->paddles[0], -1, dt);

        
        if (!game->single_player) { // If there is a second player, allow input to determine paddle 2 movement
            if (game->input_state.keys_down[i] == KEY_K) move_paddle(&game->paddles[1],  1, dt);
            if (game->input_state.keys_down[i] == KEY_I) move_paddle(&game->paddles[1], -1, dt);
        }
    }


    //---------------------------------------------------------------------------------------------------------


    if (game->single_player) {
        cpu_move_paddle(&game->paddles[1], game->ball.position, game->ball.radius, game->ball.direction.x, dt);

    }
}


void reset_game_on_score(Game* game, Paddle paddle) {
    
    reset_ball(&game->ball);


    //------------------------------------------------


    reset_paddle(&game->paddles[0]);
    reset_paddle(&game->paddles[1]);

    game->paddle_last_scored = paddle.player_number;


    //------------------------------------------------


    game->time = 0.0f;

    pause_unpause_timer(&game->reset_animation_timer);
    
}


void step_physics(Game* game, float dt) {

    if (game->screen_event != PLAY) return;

    step_timer(&game->reset_animation_timer, dt);

    if (game->reset_animation_timer.finished && !game->reset_animation_timer.paused) {
        float direction = -1;
        if (game->paddle_last_scored == PLAYER_ONE) direction = 1;

        game->ball.direction.x = direction;
        reset_timer(&game->reset_animation_timer);
        pause_unpause_timer(&game->reset_animation_timer);
    }

    if (!game->reset_animation_timer.paused) return;


    //--------------------------------------------------------------------------------


    game->time += dt;
    game->draw_dt = dt;

    update_paddle(&game->paddles[0]);
    update_paddle(&game->paddles[1]);

    update_ball(&game->ball, game->paddles, dt);

    Paddle* scored = check_score(game->ball, game->paddles);

    if (scored == NULL) return;


    //--------------------------------------------------------------------------------


    score(scored);
    reset_game_on_score(game, *scored);


    if (game->paddles[0].score == 10 || game->paddles[1].score == 10) { // The game ends when someone reaches a score of ten
        game->screen_event = WIN;

    } 
}


Paddle* check_score(Ball ball, Paddle* paddles) {

    if (ball.position.x - ball.radius > SCREEN_WIDTH) return &paddles[0]; // If the ball is past the right side of screen, player one scores
    
    if (ball.position.x + ball.radius < 0) return &paddles[1]; // If the ball is past the left side of screen, player two scores

    return NULL; // If no one has scored, return null pointer. Important to remember to always handle null case with return value

}


void draw_fps(float dt) {

    const char* text = TextFormat("fps = %.0f", 1.0f / (roundf(dt * 1000) / 1000));
    const int font_size = 30;

    const float text_x = (SCREEN_WIDTH - MeasureText(text, font_size)) / 2.0f;
    const float text_y = 70.0f - font_size;


    //------------------------------------------------------------------------


    DrawText(text, text_x, text_y, font_size, GREEN);

}


void draw_markers() {
    
    const int markers = 10;
    const float marker_length = SCREEN_HEIGHT / 21.0f;


    //---------------------------------------------------------------------------------------------------


    for (int i = 3; i < markers * 2 - 1; i += 2) {
        DrawRectangle((float) SCREEN_WIDTH / 2.0f - 2.0f, i * marker_length, 4.0f, marker_length, WHITE);

    }
}


void draw_time(float time) {

    float timer_factor = time / 60.0f;
    if (timer_factor >= 1.0f) timer_factor = 1.0f;


    //-----------------------------------------------------------------------------


    const float r_offset = 25 * timer_factor;
    const float g_offset = 214 * timer_factor;
    const float b_offset = 200 * timer_factor;
    Color time_color = { 255 - r_offset, 255 - g_offset, 255 - b_offset, 255 };


    //-----------------------------------------------------------------------------


    const char* text = TextFormat("%.1f", time);
    const int font_size = 40;

    const float text_x = (SCREEN_WIDTH - MeasureText(text, font_size)) / 2.0f;
    const float text_y = SCREEN_HEIGHT - 70.0f;

    //-----------------------------------------------------------------------------


    DrawText(text, text_x, text_y, font_size, time_color);

}


void draw_play(Game game) {

    draw_fps(game.draw_dt);
    draw_markers();
    draw_time(game.time);


    //---------------------------------------------------------------------------------------------


    draw_paddle(game.paddles[0]);
    draw_paddle(game.paddles[1]);

    draw_ball(game.ball, game.reset_animation_timer.elapsed, game.reset_animation_timer.paused);


    //---------------------------------------------------------------------------------------------


    draw_paddle_score(game.paddles[0].player_number, game.paddles[0].score);
    draw_paddle_score(game.paddles[1].player_number, game.paddles[1].score);
    
    

}

void draw_win(Game game) {

    float win_text_y = (SCREEN_HEIGHT - 70.0f) / 2.0f - 200.0f;


    if (game.paddle_last_scored == PLAYER_ONE) {
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
    const char* score_player_one = TextFormat("%d", game.paddles[0].score);
    const char* score_player_two = TextFormat("%d", game.paddles[1].score);

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

    float play_text_x = (SCREEN_WIDTH - MeasureText("Press 1 to play singleplayer", 30)) / 2;
    float play_text_y = (SCREEN_HEIGHT - 30.0f) / 2.0f - 50.0f;
    DrawText("Press 1 to play singleplayer", play_text_x, play_text_y, 30, GREEN);

    float multi_paddle1_text_x = ( SCREEN_WIDTH - MeasureText("Player 1 uses keys w and s", 20)) / 2.0f;
    float multi_paddle1_text_y = play_text_y + 50.0f;
    DrawText("Player 1 uses keys w and s", multi_paddle1_text_x, multi_paddle1_text_y, 20, BLUE);

    

    float play2_text_x = (SCREEN_WIDTH - MeasureText("Press 2 to play multiplayer", 30)) / 2;
    float play2_text_y = play_text_y + 100.0f;
    DrawText("Press 2 to play multiplayer", play2_text_x, play2_text_y, 30, GREEN);

    float paddle1_text_x = (8.0f * SCREEN_WIDTH / 20.0f) - MeasureText("Player 1 uses keys w and s", 20) / 2.0f;
    float paddle1_text_y = play2_text_y + 50.0f;
    DrawText("Player 1 uses keys w and s", paddle1_text_x, paddle1_text_y, 20, BLUE);

    float paddle2_text_x = (12.0f * SCREEN_WIDTH / 20.0f) - MeasureText("Player 2 uses keys i and k", 20) / 2.0f;
    float paddle2_text_y = play2_text_y + 50.0f;
    DrawText("Player 2 uses keys i and k", paddle2_text_x, paddle2_text_y, 20, PURPLE);

    float exit_text_x = (SCREEN_WIDTH - MeasureText("Press BACKSPACE to exit", 30)) / 2;
    float exit_text_y = play2_text_y + 200.0f;
    DrawText("Press BACKSPACE to exit", exit_text_x, exit_text_y, 30, RED);
    
}

void draw_game(Game game) {

    BeginDrawing();

        ClearBackground(BLACK);

        if (game.screen_event == MENU) draw_menu();
        else if (game.screen_event == PLAY) draw_play(game);
        else if (game.screen_event == WIN) draw_win(game);

    EndDrawing();

}
