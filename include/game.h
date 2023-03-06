#include "raylib.h"

#include "paddle.h"
#include "ball.h"
#include "input.h"

#ifndef GAME_H
#define GAME_H

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

Game initial_game_state();

void reset_game_on_score(Game* game, Paddle paddle);

void step_physics(Game* game, float dt);

// Checks if any scoring has happened. Returns pointer to paddle struct if a player has scored, or a NULL pointer if no player has scored.
Paddle* check_score(Ball ball, Paddle* paddles);

void draw_play(Game* game);

void draw_win(Game* game);

void draw_menu();

void draw_game(Game* game);

void handle_input(Game* game, float dt);

#endif