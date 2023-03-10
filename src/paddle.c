#include "../include/raylib.h"

#include "../include/paddle.h"


Paddle create_paddle(Player player_number) {

    Paddle paddle;

    const int width  = 40;
    const int height = 200; // Paddles are 40 x 200 pixels.

    const float y = (SCREEN_HEIGHT - height) / 2.0f; // Paddles start vertically centered.

    const float speed = 300.0f; // Paddles travel 300 pixels / second.


    //------------------------------------------------------------


    int paddle_margin = 50; // Paddles are 50 pixels from the edge of the screen.
    
    float x = paddle_margin; // Player one is on the left side.
    if (player_number == PLAYER_TWO) x = SCREEN_WIDTH - x - width; // Player two is on the right side.


    //------------------------------------------------------------


    paddle.player_number = player_number;

    paddle.position = (Vector2) { x, y };
    paddle.size     = (Vector2) { width, height };

    paddle.left   = x;
    paddle.right  = x + width;
    paddle.top    = y;
    paddle.bottom = y + height;

    paddle.speed = speed;

    paddle.score = 0;


    //------------------------------------------------------------


    return paddle;
}


void move_paddle(Paddle* paddle, int direction, float dt) { paddle->position.y += paddle->speed * direction * dt; }


void cpu_move_paddle(Paddle* paddle, Vector2 ball_position, float ball_radius, float ball_direction, float dt) {
    float direction = 0;

    if (ball_direction == 1) {

        if (ball_position.y - 2.0f * ball_radius < paddle->top) direction = -1; // Chase after ball
        else if (ball_position.y + 2.0f * ball_radius > paddle->bottom) direction = 1;

        if (ball_position.x < 3.0f * SCREEN_WIDTH / 4.0f) { // If the ball is not past three 3rds of the screen, limit paddle movement
            if (direction == -1 && paddle->top < 150.0f) direction = 0;
            else if (direction == 1 && paddle->bottom > SCREEN_HEIGHT - 150.0f) direction = 0;
        }

    } else { // If the ball is moving away from the paddle, return to center screen
    
        float paddle_middle = paddle->position.y + paddle->size.y / 2.0f;
        float epsilon = 5.0f; // Not nessecary to move to exactly the middle. Without an epsilon, paddle could get stuck going back and forth trying to be exactly centered

        if (paddle_middle < SCREEN_HEIGHT / 2.0f - epsilon) direction = 1;
        else if (paddle_middle > SCREEN_HEIGHT / 2.0f + epsilon) direction = -1;

    }


    //----------------------------------------------------------------------------------------


    paddle->position.y += paddle->speed * 2.0f * direction * dt;

}


void update_paddle(Paddle* paddle) {
    
    paddle->left = paddle->position.x;
    paddle->right = paddle->position.x + paddle->size.x;
    paddle->top = paddle->position.y;
    paddle->bottom = paddle->position.y + paddle->size.y;


    //-------------------------------------------------------------------------------------------


    if (paddle->top < 0) paddle->position.y = 0;
    else if (paddle->bottom > SCREEN_HEIGHT) paddle->position.y = SCREEN_HEIGHT - paddle->size.y; // Clamps postition onto screen

}


void reset_paddle(Paddle* paddle) { paddle->position.y = (SCREEN_HEIGHT - paddle->size.y) / 2.0f; }


void score(Paddle* paddle) { paddle->score++; }


void draw_paddle(Paddle paddle, Color accent_color, Color paddle_color) {

    const int accent_width  = paddle.size.x;
    const int accent_height = 4; // Accents are 20 pixels tall

    const float accent_x = paddle.position.x;

    const float third_of_paddle = paddle.size.y / 3.0f;
    const float accent_one_y = paddle.position.y + third_of_paddle        - accent_height / 2.0f;
    const float accent_two_y = paddle.position.y + third_of_paddle * 2.0f - accent_height / 2.0f; // Paddle has two evenly spaced accents


    //-------------------------------------------------------------------------------------------


    DrawRectangleV(paddle.position, paddle.size, paddle_color);

    DrawRectangle(accent_x, accent_one_y, accent_width, accent_height, accent_color);
    DrawRectangle(accent_x, accent_two_y, accent_width, accent_height, accent_color);
    
}


void draw_paddle_score(Player player_number, int score, Color score_color, Color start_color) {

    float score_factor = score / 9.0f; // The closer the score is to 9, the more colorful the text is
    if (score_factor >= 1.0f) score_factor = 1.0f; // Win screen still shows text, so color at score = 10 is same as score = 9

    const int inverse_r = 255 - score_color.r;
    const int inverse_g = 255 - score_color.g;
    const int inverse_b = 255 - score_color.b;
    const int inverse_a = 255 - score_color.a;

    score_color.r = start_color.r - inverse_r * score_factor; 
    score_color.g = start_color.g - inverse_g * score_factor; 
    score_color.b = start_color.b - inverse_b * score_factor;
    score_color.a = start_color.a - inverse_a * score_factor; // The higher the score, the closer the score text is to player color


    //-----------------------------------------------------------------------


    const char* text = TextFormat("%d", score);

    const int font_size = 60;

    const float text_width = MeasureText(text, font_size);

    float fourths = 1.0f; // Player one score is spaced at one fourth of the screen
    if (player_number == PLAYER_TWO) fourths = 3.0f; // Player two score is spaced at three fourths of the screen

    const float text_x = (SCREEN_WIDTH * fourths + 2.0f * text_width) / 4.0f;
    const float text_y = 50.0f;


    //-----------------------------------------------------------------------


    DrawText(text, text_x, text_y, font_size, score_color);

}
