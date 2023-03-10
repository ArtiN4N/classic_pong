#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include "paddle.h"
#include "ball.h"
#include "input.h"
#include "timer.h"

//-- SCREEN DIMENSIONS --
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
//-----------------------

// Enum to designate menu, win, and play screens.
typedef enum ScreenEvent { MENU = 0, PLAY, WIN } ScreenEvent;

// Enum for color palette colors.
typedef enum PaletteColors { FOREGROUND = 0, PLAYER1, PLAYER2, PRIMARY, SECONDARY, EXIT, BACKGROUNDACCENT, BACKGROUND, BASICBLACK, BASICWHITE } PaletteColors;

// The game state stores all relevant data to how to program can adapt and change.
typedef struct {
    Paddle paddles[2];

    Ball ball;

    InputState input_state;

    bool single_player;

    float time;
    float draw_dt;

    Timer reset_animation_timer;

    Player paddle_last_scored;

    bool close;

    ScreenEvent screen_event;

    Sound menuSelect;
    Sound fxWin;

    Color palette[10];
} Game;

// Creates a default game state. Returns a Game struct.
Game initial_game_state();

void handle_input(Game* game, float dt);

// Resets game to a default state on score. Paddles and ball are reset, and a short animation is played.
void reset_game_on_score(Game* game, Paddle paddle);

void step_physics(Game* game, float dt);

// Checks if any scoring has happened. Returns pointer to paddle struct if a player has scored, or a NULL pointer if no player has scored.
Paddle* check_score(Ball ball, Paddle* paddles);

void draw_fps(float dt, Color color);

// Draws halfway court markers during play
void draw_markers(Color color);

// Draws game clock
void draw_time(float time, Color start_color, Color end_color);

void draw_play(Game game);

void draw_win(Game game);

void draw_menu(Game game);

void draw_game(Game game);

// Unloads all allocated resources in game
void unload_resources(Game game);

#endif