#include "include/timer.h"


void reset_timer(Timer* timer) {
    timer->elapsed = 0.0f;
}

void step_timer(Timer* timer, float time) {
    if (timer->paused || timer->elapsed == timer->length) return;

    timer->elapsed += time;
    if (timer->elapsed > timer->length) timer->elapsed = timer->length;
}