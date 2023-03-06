#include "stdbool.h"

#ifndef TIMER_H
#define TIMER_H

// Timer to keep track of a single event. Can be paused and set to loop.
typedef struct {
    float elapsed;
    float length;
    
    bool paused;
    bool loop;
    bool finished;
} Timer;

// Creates a default timer. Starts at 0, but is not paused. Returns a timer struct.
Timer create_timer(float length, bool loop);

void reset_timer(Timer* timer);

void pause_unpause_timer(Timer* timer);

// Updates timer. Does nothing if timer is paused. Updates timer to reflect if it has completed, and loops if the timer is set to loop.
void step_timer(Timer* timer, float time);

#endif