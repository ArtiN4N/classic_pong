#include "../include/timer.h"

Timer create_timer(float length, bool loop) {
    Timer timer = { 0.0f, length, false, loop, false };

    return timer;
}

void pause_unpause_timer(Timer* timer) {
    timer->paused = !timer->paused;
}

void reset_timer(Timer* timer) {
    timer->finished = false;
    timer->elapsed = 0.0f;
}

void step_timer(Timer* timer, float time) {
    if (timer->paused || timer->finished) return;

    timer->elapsed += time;
    if (timer->elapsed > timer->length) {
        timer->elapsed = timer->length;
        timer->finished = true;
    }
}