//
// Created by Pelikán on 20.11.2021.
//

#ifndef SERVER_STATEMACHINE_H
#define SERVER_STATEMACHINE_H

typedef enum e_state {
    ST_NOT_ALLOWED = 0,
    ST_INIT,
    ST_WAITING,
    ST_WORKING,
    ST_PAUSE,
    ST_QUIT
} State;

typedef enum e_event {
    EV_LOGIN = 0,
    EV_GUESS = 1,
    EV_FINGERS = 2,
    EV_WRONG = 3,
} Event;

#endif //SERVER_STATEMACHINE_H
