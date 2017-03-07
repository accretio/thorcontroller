#include <stdio.h>
#include <stdlib.h>

#include "state.h"

pthread_cond_t state_cont = PTHREAD_COND_INITIALIZER;
pthread_mutex_t state_mtx = PTHREAD_MUTEX_INITIALIZER;

// a bit of a bandaid for now
struct state_t global_state = { 0 }; 
