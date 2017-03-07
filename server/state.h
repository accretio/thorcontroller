#ifndef STATE_H
#define STATE_H

struct state_t
{
  int pwm; 
};

pthread_mutex_t state_mtx;
struct state_t global_state;
  
#endif
