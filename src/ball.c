#include "include/raylib.h"
#include "include/ball.h"

//-- SCREEN DIMENSIONS --
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
//-----------------------

Ball create_ball() {
    const float x = SCREEN_WIDTH / 2.0f;
    const float y = SCREEN_HEIGHT / 2.0f;

    const float start_speed = 425.0f;
    const float radius = 20.0f;


    //------------------------------------------------------------


    Ball ball;

    ball.position = (Vector2) { x, y };
    ball.velocity = (Vector2) { -1.0f, 0.0f };

    ball.speed = start_speed;
    ball.radius = radius;


    //------------------------------------------------------------


    return ball;
}