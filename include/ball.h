#ifndef BALL_H
#define BALL_H

#include "raylib.h"
#include "timer.h"
#include "paddle.h"

#include "stdbool.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800

// 2D circle with elastic collisions. Speed increases on some defined interval.
typedef struct {
    Vector2 position;
    Vector2 previous_position;

    Vector2 direction;

    float speed;
    float radius;

    Timer speedup_timer;
} Ball;

// Creates a default ball. Starts in the middle of the screen, with its base speed, directed towards player one. Returns ball struct.
Ball create_ball();

void speedup_ball(Ball* ball);

// Resets ball. Does the same thing as create_ball, without returning a ball struct.
void reset_ball(Ball* ball);

// Updates kinematics of ball, and checks for collision.
void update_ball(Ball* ball, Paddle* paddles, float dt);

// Detects collision between ball and a paddle. Also handles fixing the balls position. Returns a boolean representing if a collision has happened.
bool detect_collision(Ball* ball, Paddle paddle);

#endif