#include "include/raylib.h"
#include "include/paddle.h"

//-- SCREEN DIMENSIONS --
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
//-----------------------

Paddle create_paddle(Player player_number) {

    const int width  = 40;
    const int height = 200; // Paddles are 40 x 200 pixels.

    const float y = (SCREEN_HEIGHT - height) / 2.0f; // Paddles start vertically centered.

    const float speed = 300.0f; // Paddles travel 300 pixels / second.


    //--------------------------------------------------------------------------------------------------------------------------------------------------


    Paddle paddle;

    int paddle_margin = 50; // Paddles are 50 pixels from the edge of the screen.

    // Player one is on the left side.
    // Player two is on the right side.
    float x = paddle_margin; 
    if (player_number == PLAYER_TWO) x = SCREEN_WIDTH - x - width;


    //--------------------------------------------------------------------------------------------------------------------------------------------------


    paddle.player_number = player_number;

    paddle.position = (Vector2) { x, y };
    paddle.size     = (Vector2) { width, height };

    paddle.left   = x;
    paddle.right  = x + width;
    paddle.top    = y;
    paddle.bottom = y + height;

    paddle.speed = speed;

    paddle.score = 0;


    //--------------------------------------------------------------------------------------------------------------------------------------------------


    return paddle;
}


void draw_paddle(Paddle paddle) {

    const Color paddle_color = WHITE; // Paddles are white.

    Color accent_color = BLUE; // Player one has blue accent colors.
    if (paddle.player_number == PLAYER_TWO) accent_color = PURPLE; // player two has purple accent colors.

    const int accent_width  = paddle.size.x;
    const int accent_height = 20; // Accents are 20 pixels tall

    const float accent_x = paddle.position.x;

    const float third_of_paddle = paddle.size.y / 3.0f;
    const float accent_one_y = paddle.position.y + third_of_paddle        - accent_height / 2.0f;
    const float accent_two_y = paddle.position.y + third_of_paddle * 2.0f - accent_height / 2.0f; // Paddle has two evenly spaced accents


    //---------------------------------------------------------------------------------------------------------


    DrawRectangleV(paddle.position, paddle.size, paddle_color);

    DrawRectangle(accent_x, accent_one_y, accent_width, accent_height, accent_color);
    DrawRectangle(accent_x, accent_two_y, accent_width, accent_height, accent_color);
}

void draw_paddle_score(Player player_number, int score) {

    Color score_color = BLUE; // Player one has blue score color.
    if (player_number == PLAYER_TWO) score_color = PURPLE; // player two has purple score color.

    float score_factor = score / 9.0f;
    if (score_factor >= 1.0f) score_factor = 1.0f;

    const int inverse_r = 255 - score_color.r;
    const int inverse_g = 255 - score_color.g;
    const int inverse_b = 255 - score_color.b;

    score_color.r = 255 - inverse_r * score_factor; 
    score_color.g = 255 - inverse_g * score_factor; 
    score_color.b = 255 - inverse_b * score_factor; 


    //---------------------------------------------------------------------------------------------------------


    const char* text = TextFormat("%d", score);

    const int font_size = 60;

    const float text_width = MeasureText(text, font_size);

    float fourths = 1.0f;
    if (player_number == PLAYER_TWO) fourths = 3.0f;

    const float text_x = (SCREEN_WIDTH * fourths + 2.0f * text_width) / 4.0f;
    const float text_y = 50.0f;


    //---------------------------------------------------------------------------------------------------------


    DrawText(text, text_x, text_y, font_size, score_color);
}