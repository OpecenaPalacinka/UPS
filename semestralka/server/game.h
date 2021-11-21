//
// Created by Pelikán on 20.11.2021.
//

#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "stateMachine.h"

#define MAX_NAME_LENGTH 35

typedef struct gamers {
    char name[MAX_NAME_LENGTH];
    int numOfFingers;
    int socket;
} Gamer;

typedef struct games {
    int numOfPlayers;
    Gamer *gamers[4];
    int whoPlays;
    int secToWait;
    State state;
} Game;

#endif //SERVER_GAME_H
