#include "include/raylib.h"
#include "include/raymath.h"
#include "include/ball.h"

#include <stddef.h>

//-- SCREEN DIMENSIONS --
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
//-----------------------


Ball create_ball() {
    const float x = SCREEN_WIDTH / 2.0f;
    const float y = SCREEN_HEIGHT / 2.0f;

    const float start_speed = 425.0f;
    const float radius = 20.0f;


    //----------------------------------------


    Ball ball;

    ball.position = (Vector2) { x, y };
    ball.previous_position = ball.position;

    ball.direction = (Vector2) { -1.0f, 0.0f };

    ball.speed = start_speed;
    ball.radius = radius;

    ball.speedup_timer = (Timer) { 0.0f, 5.0f, false, false };


    //----------------------------------------


    return ball;
}


void speedup_ball(Ball* ball) {

    ball->speed *= 1.2f;
    if (ball->speed > 3000.0f) ball->speed = 3000.0f;

    reset_timer(&ball->speedup_timer);
}


void reset_ball(Ball* ball) {
    const float x = SCREEN_WIDTH / 2.0f;
    const float y = SCREEN_HEIGHT / 2.0f;

    const float start_speed = 425.0f;

    ball->position = (Vector2) { x, y };
    ball->direction = (Vector2) { 0.0f, 0.0f };

    ball->speed = start_speed;
}


void update_ball(Ball* ball, Paddle* paddles, float dt) {
    step_timer(&ball->speedup_timer, dt);

    ball->previous_position = ball->position;

    Vector2 direction_normal = Vector2Normalize(ball->direction);

    ball->position.x += direction_normal.x * ball->speed * dt;
    ball->position.y += direction_normal.y * ball->speed * dt;

    //Keeping ball on screen
    if (ball->position.y - ball->radius < 0) {
        ball->position.y = 0 + ball->radius;
        ball->direction.y *= -1;
    }
    
    if (ball->position.y + ball->radius > SCREEN_HEIGHT) {
        ball->position.y = SCREEN_HEIGHT - ball->radius;
        ball->direction.y *= -1;
    }

    bool collision = detect_collision(ball, paddles[0]);
    if (!collision) collision = detect_collision(ball, paddles[1]);

    if (collision && ball->speedup_timer.elapsed == ball->speedup_timer.length) {
        speedup_ball(ball);
        reset_timer(&ball->speedup_timer);
        
    }
}


Paddle* check_score(Ball ball, Paddle* paddles) {
    if (ball.position.x - ball.radius > SCREEN_WIDTH) return &paddles[0];
    else if (ball.position.x + ball.radius < 0) return &paddles[1];

    return NULL;
}


bool detect_collision(Ball* ball, Paddle paddle) {
    Vector2 colliding_paddle = ball->position;

    if (ball->position.x < paddle.position.x) colliding_paddle.x = paddle.position.x; // test left edge
    else if (ball->position.x > paddle.position.x + paddle.size.x) colliding_paddle.x = paddle.position.x + paddle.size.x; // right edge

    if (ball->position.y < paddle.position.y) colliding_paddle.y = paddle.position.y; // test left edge
    else if (ball->position.y > paddle.position.y + paddle.size.y) colliding_paddle.y = paddle.position.y + paddle.size.y; // right edge

    Vector2 distanceVec = { ball->position.x - colliding_paddle.x, ball->position.y - colliding_paddle.y };
    float distance = sqrt( (distanceVec.x * distanceVec.x) + (distanceVec.y * distanceVec.y) );

    if (distance <= ball->radius) {
        
        bool between_paddle = ball->previous_position.x + ball->radius > paddle.position.x && ball->previous_position.x - ball->radius < paddle.position.x + paddle.size.x;
        if (between_paddle) {
            if (ball->position.y > paddle.position.y + paddle.size.y / 2.0f) {
                ball->position.y = paddle.position.y + paddle.size.y + ball->radius;
                ball->direction.y = 1;
            } else {
                ball->position.y = paddle.position.y - ball->radius;
                ball->direction.y = -1;
            }
        }
        else if (!between_paddle) {
            ball->position.x = ball->previous_position.x;
            if (ball->position.y < paddle.position.y + paddle.size.y * 1.0f / 3.0f) {
                ball->direction.x *= -1;
                ball->direction.y = -1;
            } else if (ball->position.y > paddle.position.y + paddle.size.y * 2.0f / 3.0f) {
                ball->direction.x *= -1;
                ball->direction.y = 1;
            } else {
                ball->direction.x *= -1;
                ball->direction.y = 0;
            }
        }
        return true;
    }
    return false;
}