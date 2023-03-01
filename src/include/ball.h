#include "raylib.h"
#include "timer.h"
#include "paddle.h"

#include "stdbool.h"

#ifndef BALL_H
#define BALL_H


// The ball is hit by paddles towards their enemy's goalzone.
// When the ball passes a paddle, the opposing paddle scores.
// The ball bounces off the top and bottom of the screen, and the paddles.
typedef struct {
    Vector2 position;
    Vector2 previous_position;

    Vector2 direction;

    float speed;
    float radius;

    Timer speedup_timer;
} Ball;

// Creates a default ball.
// Paddles always: are the same size, start at the same y coordinate, start with a score of 0, and have a speed of 300 pixels per second.
// The only necessary paramter is whether or not it is player one or player two.
Ball create_ball();

void speedup_ball(Ball* ball);

void reset_ball(Ball* ball);

void update_ball(Ball* ball, Paddle* paddles, float dt);

Paddle* check_score(Ball ball, Paddle* paddles);

bool detect_collision(Ball* ball, Paddle paddle);

#endif