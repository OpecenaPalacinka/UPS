//
// Created by Pelikán on 20.11.2021.
//

#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "stateMachine.h"

#define MAX_NAME_LENGTH 15
#define MAX_INPUT_LENGTH 75

typedef struct gamers {
    char name[MAX_NAME_LENGTH];
    int numOfFingers;
    int socket;
    /* 1 připojen - 0 odpojen*/
    int connected;
} Gamer;

typedef struct games {
    int numOfPlayers;
    Gamer *gamers[4];
    int whoPlays;
    State state;
} Game;

#endif //SERVER_GAME_H
