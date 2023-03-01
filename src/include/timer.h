#ifndef TIMER_H
#define TIMER_H

typedef struct {
    float elapsed;
    float length;
    
    bool paused;
    bool loop;
} Timer;

void reset_timer(Timer* timer);

#endif