#include "stdbool.h"

#ifndef TIMER_H
#define TIMER_H

typedef struct {
    float elapsed;
    float length;
    
    bool paused;
    bool loop;
    bool finished;
} Timer;

Timer create_timer(float length, bool loop);

void reset_timer(Timer* timer);

void pause_unpause_timer(Timer* timer);

void step_timer(Timer* timer, float time);

#endif