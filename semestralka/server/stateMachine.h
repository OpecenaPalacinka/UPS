//
// Created by Pelikán on 20.11.2021.
//

#ifndef SERVER_STATEMACHINE_H
#define SERVER_STATEMACHINE_H

#define STATES_COUNT 6
#define EVENTS_COUNT 5

typedef enum e_state {
    ST_NOT_ALLOWED = 0,
    ST_INIT = 1,
    ST_WAITING = 2,
    ST_WORKING = 3,
    ST_PAUSE = 4,
    ST_QUIT = 5
} State;

typedef enum e_event {
    EV_LOGIN = 0,
    EV_GUESS = 1,
    EV_FINGERS = 2,
    EV_WRONG = 3,
    EV_PAUSE = 4,
} Event;

#endif //SERVER_STATEMACHINE_H
