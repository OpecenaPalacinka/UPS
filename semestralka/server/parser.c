//
// Created by Pelikán on 17.11.2021.
//

#include <string.h>
#include <malloc.h>

#define DELIM "|"

/*
 * Z parametru input parsuje command a parametry
 * Druhý parametr *args používá a vrátil počet argumentů
 */
char **get_args(char *input, int *args){
    char **arguments;
    char *argument;
    char *inputs;
    char *save_ptr;

    int arg = 0;

    arguments = malloc(sizeof(char **) * 8);

    for(int i = 0;i < 8;i++){
        arguments[i] = NULL;
    }

    inputs = input;
    inputs[strlen(inputs)-1] = '\0';
    save_ptr = inputs;

    while ((argument = strtok_r(save_ptr,DELIM,&save_ptr)) != NULL) {
        arguments[arg] = malloc((strlen(argument) + 2) * sizeof(char));
        strcpy(arguments[arg], argument);
        arguments[arg][strlen(argument)+1] = '\0';
        arg++;
    }
    *args = arg;
    return arguments;
}

/*
 * Převádí string na event
 */
Event setEventFromArg(char *event){
    if (strcmp("LOGIN",event) == 0){
        return EV_LOGIN;
    } else if (strcmp("GUESS",event) == 0){
        return EV_GUESS;
    } else if (strcmp("FINGERS",event) == 0){
        return EV_FINGERS;
    } else {
        printf("Wrong input! ENDING\n");
        return EV_WRONG;
    }
}

/*
 * Vytváří START command, který se rozpošle všem hráčům dané hry a odstartuje hru
 */
char * makeSTARTCommand(char *message,int numOfPlayers, Game *game){
    snprintf(message, 9,"START|%d|",numOfPlayers);

    for (int i = 0; i < numOfPlayers; ++i) {
        strcat(message,game->gamers[i]->name);
        if (i != numOfPlayers -1){
            strcat(message,"|");
        }
    }
    strcat(message,"\n");

    return message;
}

char * makeENDOFROUNDCommand(char *message, char *nextGamer, int true, char *actualGamer, int fingers, int guess){
    char yesNo[7];
    if (true == 1){
        strcpy(yesNo,"true");
    } else {
        strcpy(yesNo,"false");
    }
    snprintf(message,50,"ENDOFROUND|%s|%s|%s|%d|%d\n", nextGamer,yesNo,actualGamer,fingers,guess);
    return message;
}

char * makeENDGAMECommand(char *message, char *winGamer){
    snprintf(message,25,"ENDGAME|%s\n", winGamer);
    return message;
}

/*
 * Najde gamesku podle čísla socketu hráče
 * Používá se, když někdo pošle command, aby se zjistilo, o jakou hru se jedná
 */
int findGameBySocket(int socket, int totalNumOfGames, Game **game){
    int numOfGame = -1;
    for (int i = 0; i < totalNumOfGames; ++i) {
        for (int j = 0; j < game[i]->numOfPlayers; ++j) {
            if (game[i]->gamers[j]->socket == socket){
                numOfGame = i;
                return numOfGame;
            }
        }
    }
    return numOfGame;
}

/*
 * Najde hráče podle čísla socketu
 * Používá se, když někdo pošle command, aby se zjistilo, kdo ho poslal
 */
int findGamerBySocket(int socket, int actualGame, Game **game){
    int numOfGamer = -1;
        for (int j = 0; j < game[actualGame]->numOfPlayers; ++j) {
            if (game[actualGame]->gamers[j]->socket == socket){
                numOfGamer = j;
                return numOfGamer;
            }
        }
    return numOfGamer;
}

int nextPlayer(Game *game, int actualGamer){

    if (game->numOfPlayers - 1 >= actualGamer +1){
        printf("%d + 1\n",actualGamer);
        return actualGamer + 1;
    } else {
        printf("0\n");
        return 0;
    }
}

void removePlayerFromGame(int playerLeft, Game *game){
    char *message = malloc(sizeof(char)*50);
    char *name = game->gamers[playerLeft]->name;
    for (int i = 0; i < game->numOfPlayers-1; ++i) {
        if (i >= playerLeft){
            game->gamers[i] = game->gamers[i+1];
        }
    }
    game->numOfPlayers--;
    if (game->numOfPlayers == 1){
        makeENDGAMECommand(message,game->gamers[0]->name);
        write(game->gamers[0]->socket,message, strlen(message));
    } else {
        snprintf(message,50,"LOGOUT|%s\n",name);
        for (int i = 0; i < game->numOfPlayers; ++i) {
            write(game->gamers[i]->socket,message, strlen(message));
        }
    }

}

int findPlayerForRelog(Game **game){

}
