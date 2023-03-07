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


    game.menuSelect = LoadSound("src/resources/sfx/menu.wav");
    game.fxWin = LoadSound("src/resources/sfx/win.wav");

    SetSoundVolume(game.menuSelect, 0.3f);
    SetSoundVolume(game.fxWin, 0.5f);


    //--------------------------------------------------------------------------

    // r, g, b, a
    game.palette[FOREGROUND] = (Color) {
        255,
        236,
        214,
        255
    };
    
    game.palette[PLAYER1] = (Color) {
        208,
        129,
        89,
        255
    };

    game.palette[PLAYER2] = (Color) {
        141,
        105,
        122,
        255
    };
    
    game.palette[PRIMARY] = (Color) {
        255,
        212,
        163,
        255
    };

    game.palette[SECONDARY] = (Color) {
        255,
        170,
        94,
        255
    };
    
    game.palette[EXIT] = (Color) {
        84,
        78,
        104,
        255
    };

    game.palette[BACKGROUND] = (Color) {
        13,
        43,
        69,
        255
    };

    game.palette[BASICBLACK] = BLACK;

    game.palette[BASICWHITE] = WHITE;


    //--------------------------------------------------------------------------


    return game;

}


void handle_input(Game* game, float dt) {

    if (IsKeyDown(KEY_BACKSPACE)) { // Pressing backspace exits the game
        game->close = true;
        return;
    }

    if (game->screen_event == WIN && IsKeyDown(KEY_ESCAPE)) {  // Escape returns to menu on win screen
        PlaySoundMulti(game->menuSelect); // menu sfx
        game->screen_event = MENU;
        *game = initial_game_state();
        return;
    }

    if (game->screen_event == MENU) { // Keys one and two are used to start the game when on menu screen
        if (IsKeyDown(KEY_ONE)) {
            PlaySoundMulti(game->menuSelect); // menu sfx
            game->screen_event = PLAY;
            game->single_player = true;
        } else if (IsKeyDown(KEY_TWO)) {
            PlaySoundMulti(game->menuSelect); // menu sfx
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

    PlaySoundMulti(game->ball.fxScore); // score sfx

    reset_game_on_score(game, *scored);


    if (game->paddles[0].score == 10 || game->paddles[1].score == 10) { // The game ends when someone reaches a score of ten
        PlaySoundMulti(game->fxWin); // win sfx
        game->screen_event = WIN;
        
    } 
}


Paddle* check_score(Ball ball, Paddle* paddles) {

    if (ball.position.x - ball.radius > SCREEN_WIDTH) return &paddles[0]; // If the ball is past the right side of screen, player one scores
    
    if (ball.position.x + ball.radius < 0) return &paddles[1]; // If the ball is past the left side of screen, player two scores

    return NULL; // If no one has scored, return null pointer. Important to remember to always handle null case with return value

}


void draw_fps(float dt, Color color) {

    const char* text = TextFormat("fps = %.0f", 1.0f / (roundf(dt * 1000) / 1000));
    const int font_size = 30;

    const float text_x = (SCREEN_WIDTH - MeasureText(text, font_size)) / 2.0f;
    const float text_y = 70.0f - font_size;


    //-----------------------------------------------------------------------------


    DrawText(text, text_x, text_y, font_size, color);

}


void draw_markers(Color color) {
    
    const int markers = 10;
    const float marker_length = SCREEN_HEIGHT / 21.0f;


    //---------------------------------------------------------------------------------------------------


    for (int i = 3; i < markers * 2 - 1; i += 2) {
        DrawRectangle((float) SCREEN_WIDTH / 2.0f - 2.0f, i * marker_length, 4.0f, marker_length, color);

    }
}


void draw_time(float time, Color start_color, Color end_color) {

    float timer_factor = time / 60.0f;
    if (timer_factor >= 1.0f) timer_factor = 1.0f;


    //-----------------------------------------------------------------------------


    const float r_offset = (255 - end_color.r) * timer_factor;
    const float g_offset = (255 - end_color.g) * timer_factor;
    const float b_offset = (255 - end_color.b) * timer_factor;
    const float a_offset = (255 - end_color.a) * timer_factor;
    Color time_color = { start_color.r - r_offset, start_color.g - g_offset, start_color.b - b_offset, start_color.a - a_offset };


    //-----------------------------------------------------------------------------


    const char* text = TextFormat("%.1f", time);
    const int font_size = 40;

    const float text_x = (SCREEN_WIDTH - MeasureText(text, font_size)) / 2.0f;
    const float text_y = SCREEN_HEIGHT - 70.0f;

    //-----------------------------------------------------------------------------


    DrawText(text, text_x, text_y, font_size, time_color);

}


void draw_play(Game game) {

    draw_fps(game.draw_dt, game.palette[PRIMARY]);
    draw_markers(game.palette[FOREGROUND]);
    draw_time(game.time, game.palette[FOREGROUND], game.palette[PRIMARY]);


    //------------------------------------------------------------------------------------------


    draw_paddle(game.paddles[0], game.palette[PLAYER1], game.palette[FOREGROUND]);
    draw_paddle(game.paddles[1], game.palette[PLAYER2], game.palette[FOREGROUND]);

    draw_ball(game.ball, game.reset_animation_timer.elapsed, game.reset_animation_timer.paused, game.palette[FOREGROUND]);


    //------------------------------------------------------------------------------------------


    draw_paddle_score(game.paddles[0].player_number, game.paddles[0].score, game.palette[PLAYER1], game.palette[FOREGROUND]);
    draw_paddle_score(game.paddles[1].player_number, game.paddles[1].score, game.palette[PLAYER2], game.palette[FOREGROUND]);
    
}

void draw_win(Game game) {

    int font_size = 70;
    float win_text_y = ((SCREEN_HEIGHT - font_size) / 2.0f) - 200.0f;

    if (game.paddle_last_scored == PLAYER_ONE) {
        const char* win_text = "Player 1 Wins";
        float win_text_x = (SCREEN_WIDTH - MeasureText(win_text, font_size)) / 2.0f;
        DrawText(win_text, win_text_x, win_text_y, font_size, game.palette[PLAYER1]);
    } else {
        const char* win_text = "Player 2 Wins";
        float win_text_x = (SCREEN_WIDTH - MeasureText(win_text, font_size)) / 2.0f;
        DrawText(win_text, win_text_x, win_text_y, font_size, game.palette[PLAYER2]);        
    }

    float score_text_y = win_text_y + 100.0f;


    //---------------------------------------------------------------------------------------------


    const char* score_dash = "-";
    const char* score_player_one = TextFormat("%d", game.paddles[0].score);
    const char* score_player_two = TextFormat("%d", game.paddles[1].score);

    float dash_x = (SCREEN_WIDTH - MeasureText(score_dash, font_size)) / 2.0f;
    float player_one_x = dash_x - 25.0f - MeasureText(score_player_one, font_size);
    float player_two_x = dash_x + 25.0f + MeasureText(score_dash, font_size);

    DrawText(score_dash, dash_x, score_text_y, font_size, game.palette[FOREGROUND]);
    DrawText(score_player_one, player_one_x, score_text_y, font_size, game.palette[PLAYER1]);
    DrawText(score_player_two, player_two_x, score_text_y, font_size, game.palette[PLAYER2]);


    //---------------------------------------------------------------------------------------------


    float reset_text_x = (SCREEN_WIDTH - MeasureText("Press Escape to Return to Menu", 40)) / 2.0f;
    float reset_text_y = (SCREEN_HEIGHT - 40.0f) / 2.0f + 100.0f;
    DrawText("Press Escape to Return to Menu", reset_text_x, reset_text_y, 40, WHITE);

}

void draw_menu(Game game) {

    int font_size = 80;
    const char* title_text = "classic_pong";
    const float title_text_x = (SCREEN_WIDTH - MeasureText(title_text, font_size)) / 2.0f;
    const float title_text_y = ((SCREEN_HEIGHT - font_size) / 2.0f) - 200.0f;
    DrawText("classic_pong", title_text_x, title_text_y, font_size, game.palette[FOREGROUND]);


    //--------------------------------------------------------------------------------------------------


    font_size = 30;
    const char* play_text = "Press 1 to play singleplayer";
    float play_text_x = (SCREEN_WIDTH - MeasureText(play_text, font_size)) / 2.0f;
    float play_text_y = ((SCREEN_HEIGHT - font_size) / 2.0f) - 50.0f;
    DrawText(play_text, play_text_x, play_text_y, font_size, game.palette[PRIMARY]);


    //--------------------------------------------------------------------------------------------------


    font_size = 20;
    const char* multi_paddle1_text = "Player 1 uses keys w and s";
    float multi_paddle1_text_x = ( SCREEN_WIDTH - MeasureText(multi_paddle1_text, font_size)) / 2.0f;
    float multi_paddle1_text_y = play_text_y + 50.0f;
    DrawText(multi_paddle1_text, multi_paddle1_text_x, multi_paddle1_text_y, font_size, game.palette[PLAYER1]);

    
    //--------------------------------------------------------------------------------------------------


    font_size = 30;
    const char* play2_text = "Press 2 to play multiplayer";
    float play2_text_x = (SCREEN_WIDTH - MeasureText(play2_text, font_size)) / 2.0f;
    float play2_text_y = play_text_y + 100.0f;
    DrawText(play2_text, play2_text_x, play2_text_y, font_size, game.palette[PRIMARY]);


    //--------------------------------------------------------------------------------------------------


    font_size = 20;
    const char* paddle1_text = "Player 1 uses keys w and s";
    float paddle1_text_x = (8.0f * SCREEN_WIDTH / 20.0f) - MeasureText(paddle1_text, font_size) / 2.0f;
    float paddle1_text_y = play2_text_y + 50.0f;
    DrawText(paddle1_text, paddle1_text_x, paddle1_text_y, font_size, game.palette[PLAYER1]);


    //--------------------------------------------------------------------------------------------------


    font_size = 20;
    const char* paddle2_text = "Player 2 uses keys i and k";
    float paddle2_text_x = (12.0f * SCREEN_WIDTH / 20.0f) - MeasureText(paddle2_text, font_size) / 2.0f;
    float paddle2_text_y = play2_text_y + 50.0f;
    DrawText(paddle2_text, paddle2_text_x, paddle2_text_y, font_size, game.palette[PLAYER2]);


    //--------------------------------------------------------------------------------------------------


    font_size = 30;
    const char* exit_text = "Press BACKSPACE to exit";
    float exit_text_x = (SCREEN_WIDTH - MeasureText(exit_text, font_size)) / 2.0f;
    float exit_text_y = play2_text_y + 200.0f;
    DrawText(exit_text, exit_text_x, exit_text_y, font_size, game.palette[SECONDARY]);
    
}

void draw_game(Game game) {

    BeginDrawing();

        ClearBackground(game.palette[BACKGROUND]);

        if (game.screen_event == MENU) draw_menu(game);
        else if (game.screen_event == PLAY) draw_play(game);
        else if (game.screen_event == WIN) draw_win(game);

    EndDrawing();

}


void unload_resources(Game game) {

    ball_unload_resources(game.ball);


    //-------------------------------


    UnloadSound(game.menuSelect);
    UnloadSound(game.fxWin);

}