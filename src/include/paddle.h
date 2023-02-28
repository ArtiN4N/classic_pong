#include "raylib.h"

#ifndef PADDLE_H
#define PADDLE_H

typedef enum Player { PLAYER_ONE = 0, PLAYER_TWO } Player;

// Paddles are controlled by players to interact with the game. 
// When the ball hits a paddle, it bounces off of the paddle depending on where it hit the paddle.
// When the ball hits the middle of the paddle, it bounces straight off.
// When the ball hits the top of the paddle, it bounces up.
// Finally, when the ball hits the bottom of the paddle, it bounces down.
// Paddles can only move up and down, and are limited by the screen dimensions.
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


// Creates a default paddle.
// Paddles always: are the same size, start at the same y coordinate, start with a score of 0, and have a speed of 300 pixels per second.
// The only necessary paramter is whether or not it is player one or player two.
Paddle create_paddle(Player player_number);

void move_paddle(Paddle* paddle, int direction, float dt);

void reset_paddle(Paddle* paddle);

void score(Paddle* paddle);

void draw_paddle(Paddle paddle);

void draw_paddle_score(Player player_number, int score);

#endif