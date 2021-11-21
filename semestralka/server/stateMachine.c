#include <stdio.h>
#include <stdlib.h>

#define STATES_COUNT 6
#define EVENTS_COUNT 4

const char * const state_str[] = {
        [ST_NOT_ALLOWED] = "not allowed",
        [ST_INIT] = "init",
        [ST_WAITING] = "waiting",
        [ST_WORKING]  = "working",
        [ST_PAUSE]  = "pause",
        [ST_QUIT] = "quit"
};

const char * const event_str[] = {
        [EV_LOGIN] = "login",
        [EV_GUESS] = "clients guess",
        [EV_FINGERS] = "clients fingers",
};

State transitions[STATES_COUNT][EVENTS_COUNT] = {
        //[ST_INIT][EV_WAIT] = ST_WAITING,
        //[ST_WAITING][EV_WORK] = ST_WORKING,
        //[ST_WAITING][EV_PAUSE] = ST_PAUSE
};

void print_states(){
    for (int i=0; i<STATES_COUNT;i++){
        printf("State: %s(%d)\n", state_str[i], i);
    }
}

void print_state(State state){
    printf("%s\n", state_str[state]);
}

int allowed_transition(State state, Event event) {
    return transitions[state][event];
}


