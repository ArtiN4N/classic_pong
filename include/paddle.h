#ifndef PADDLE_H
#define PADDLE_H

#include "raylib.h"

//-- SCREEN DIMENSIONS --
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
//-----------------------

typedef enum Player { PLAYER_ONE = 0, PLAYER_TWO } Player;

// Paddles are controlled by players to interact with the game. 
// When the ball hits a paddle, it bounces off of the paddle depending on where it hit the paddle.
// When the ball hits the middle of the paddle, it bounces straight off.
// When the ball hits the top of the paddle, it bounces up.
// When the ball hits the bottom of the paddle, it bounces down.
// Paddles can move up and down, and are limited by the screen dimensions.
typedef struct { 
    Player player_number;

    Vector2 position;
    Vector2 size;

    float left;
    float right;
    float top;
    float bottom;

    float speed;

    int score;
} Paddle;


// Creates a default paddle. Returns a paddle struct.
Paddle create_paddle(Player player_number);

void move_paddle(Paddle* paddle, int direction, float dt);

// CPU AI to stop opponent from scoring.
void cpu_move_paddle(Paddle* paddle, Vector2 ball_position, float ball_radius, float ball_direction, float dt);

void update_paddle(Paddle* paddle);

void reset_paddle(Paddle* paddle);

// Update a paddle that has scored.
void score(Paddle* paddle);

void draw_paddle(Paddle paddle);

void draw_paddle_score(Player player_number, int score);

#endif