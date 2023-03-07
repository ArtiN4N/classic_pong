#include "../include/raylib.h"
#include "../include/raymath.h"

#include "../include/ball.h"

#include <stddef.h>


Ball create_ball() {

    const float x = SCREEN_WIDTH  / 2.0f;
    const float y = SCREEN_HEIGHT / 2.0f;

    const float direction_x = -1.0f;
    const float direction_y =  0.0f;

    const float start_speed = 425.0f;

    const float radius = 20.0f;

    const float speedup_length = 5.0f; // in seconds

    const bool speedup_loop = false;

    //--------------------------------------------------------------


    Ball ball;

    ball.position = (Vector2) { x, y };
    ball.previous_position = (Vector2) { x, y };

    ball.direction = (Vector2) { direction_x, direction_y };

    ball.speed = start_speed;

    ball.radius = radius;

    ball.speedup_timer = create_timer(speedup_length, speedup_loop);


    //--------------------------------------------------------------


    ball.fxHit = LoadSound("resources/sfx/hit.wav");
    ball.fxScore = LoadSound("resources/sfx/score.wav");

    SetSoundVolume(ball.fxHit, 0.5f);
    SetSoundVolume(ball.fxScore, 0.5f);


    //--------------------------------------------------------------


    return ball;

}


void speedup_ball(Ball* ball) {

    ball->speed *= 1.2f;
    if (ball->speed > 3000.0f) ball->speed = 3000.0f; // Clamp ball speed to 3000 pixels /s


    //-----------------------------------------------


    reset_timer(&ball->speedup_timer);

}


void reset_ball(Ball* ball) {

    const float x = SCREEN_WIDTH / 2.0f;
    const float y = SCREEN_HEIGHT / 2.0f;

    const float start_speed = 425.0f;


    //-----------------------------------------


    ball->position = (Vector2) { x, y };
    ball->direction = (Vector2) { 0.0f, 0.0f };

    ball->speed = start_speed;

}


void update_ball(Ball* ball, Paddle* paddles, float dt) {

    step_timer(&ball->speedup_timer, dt);


    //-------------------------------------------------------------


    ball->previous_position = ball->position;

    Vector2 direction_normal = Vector2Normalize(ball->direction); // Keeps direction vector magnitude clamped to 1

    ball->position.x += direction_normal.x * ball->speed * dt;
    ball->position.y += direction_normal.y * ball->speed * dt;


    //-------------------------------------------------------------

    
    if (ball->position.y - ball->radius < 0) { // Top of screen collision
        ball->position.y = 0 + ball->radius;
        ball->direction.y *= -1;

        PlaySound(ball->fxHit); // hit sfx
    }
    
    if (ball->position.y + ball->radius > SCREEN_HEIGHT) { // Bottom of screen collision
        ball->position.y = SCREEN_HEIGHT - ball->radius;
        ball->direction.y *= -1;

        PlaySound(ball->fxHit); // hit sfx
    }


    //-------------------------------------------------------------


    bool collision = detect_collision(ball, paddles[0]);
    if (!collision) collision = detect_collision(ball, paddles[1]);

    if (collision) {
        PlaySound(ball->fxHit); // hit sfx

        if (ball->speedup_timer.finished) { // If the timer is over, speed up the ball
            speedup_ball(ball);
            reset_timer(&ball->speedup_timer);

        }
    }
}


void draw_ball(Ball ball, float animation_elapsed, bool animation_paused, Color color) {

    int ball_opac = 255;
    if (!animation_paused) {
        int opac_frame = animation_elapsed * 10.0f;
        if (opac_frame % 2 == 0) ball_opac = 255 / 2.0f;
    }

    color.a = ball_opac;


    //--------------------------------------------------------------------------


    DrawCircleV(ball.position, ball.radius, color); // Draw the ball
    
}


bool detect_collision(Ball* ball, Paddle paddle) {

    // Create vector to store which side of the paddle is colliding with the ball
    // The side of the paddle colliding witht the ball is used to find the distance between the ball and the surface of the rectangle
    Vector2 colliding_paddle = ball->position; 


    //--------------------------------------------------------------------------------------------------------------------


    if (ball->position.x < paddle.position.x) colliding_paddle.x = paddle.position.x; // test if ball is hitting left edge of paddle
    else if (ball->position.x > paddle.position.x + paddle.size.x) colliding_paddle.x = paddle.position.x + paddle.size.x; // test if ball is hitting right edge of paddle

    if (ball->position.y < paddle.position.y) colliding_paddle.y = paddle.position.y; // test if ball is hitting top edge of paddle
    else if (ball->position.y > paddle.position.y + paddle.size.y) colliding_paddle.y = paddle.position.y + paddle.size.y; // test if ball is hitting bottom edge of paddle


    //--------------------------------------------------------------------------------------------------------------------


    Vector2 distanceVec = { ball->position.x - colliding_paddle.x, ball->position.y - colliding_paddle.y };
    float distance = sqrt( (distanceVec.x * distanceVec.x) + (distanceVec.y * distanceVec.y) ); // Distance between ball and paddle surface


    //--------------------------------------------------------------------------------------------------------------------


    if (distance > ball->radius) return false; // Collision does not occur


    //--------------------------------------------------------------------------------------------------------------------

    
    bool past_paddle_left = ball->previous_position.x + ball->radius > paddle.position.x;
    bool past_paddle_right = ball->previous_position.x - ball->radius < paddle.position.x + paddle.size.x;
    
    bool between_paddle = past_paddle_left && past_paddle_right;


    //--------------------------------------------------------------------------------------------------------------------


    if (between_paddle) { // Tests if ball is colliding with top or bottom of paddle
        if (ball->position.y > paddle.position.y + paddle.size.y / 2.0f) { // Bottom of paddle
            ball->position.y = paddle.position.y + paddle.size.y + ball->radius;
            ball->direction.y = 1;
        } else { // Top of paddle
            ball->position.y = paddle.position.y - ball->radius;
            ball->direction.y = -1;
        }

        return true; // Collision occurs
    } // Otherwise, ball is colliding with front of paddle


    //--------------------------------------------------------------------------------------------------------------------


    ball->position.x = ball->previous_position.x;
    if (ball->position.y < paddle.position.y + paddle.size.y * 1.0f / 3.0f) { // Colliding with top of paddle's front
        ball->direction.x *= -1;
        ball->direction.y = -1;
    } else if (ball->position.y > paddle.position.y + paddle.size.y * 2.0f / 3.0f) { // Colliding with bottom of paddle's front
        ball->direction.x *= -1;
        ball->direction.y = 1;
    } else {  // Colliding with middle of paddle's front
        ball->direction.x *= -1;
        ball->direction.y = 0;
    }

    return true; // Collision occurs

}

void ball_unload_resources(Ball ball) {

    UnloadSound(ball.fxHit);
    UnloadSound(ball.fxScore);

}