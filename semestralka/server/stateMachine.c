#include <stdio.h>
#include <stdlib.h>

const char * const state_str[] = {
        [ST_NOT_ALLOWED] = "not allowed",
        [ST_WAITING] = "waiting",
        [ST_GUESSING]  = "working",
};

const char * const event_str[] = {
        [EV_LOGIN] = "login",
        [EV_GUESS] = "clients guess",
        [EV_FINGERS] = "clients fingers",
};

State transitions[STATES_COUNT][EVENTS_COUNT] = {
        [ST_INIT][EV_LOGIN] = ST_INIT,
        [ST_WAITING][EV_GUESS] = ST_GUESSING,
        [ST_GUESSING][EV_FINGERS] = ST_GUESSING
};

/**
 * Vrací hodnotu v matici, výsledek
 * @param state Aktuální stav
 * @param event Událost, která se děje
 * @return Vrací hodnotu mez 0-5
 */
int allowed_transition(State state, Event event) {
    return transitions[state][event];
}


