//
// Created by Pelikán on 17.11.2021.
//

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define DELIM "|"

/**
 * Z parametru input parsuje command a parametry
 * Druhý parametr *args používá a vrátil počet argumentů
 * @param input Ukazatel na zprávu
 * @param args Počet argumentů
 * @return Vrací pole argumentů
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

/**
 * Převádí string na event
 * @param event event
 * @return Vrací Event
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

/**
 * Vytváří START command, který se rozpošle všem hráčům dané hry a odstartuje hru
 * @param message Ukazatel na zprávu
 * @param numOfPlayers Počet hráčů
 * @param game Hra
 * @return Vrací zprávu
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

/**
 * Vytváří zprávu o konci kola
 * @param message Ukazatel na zprávu
 * @param nextGamer Jméno hráče, který je další na tahu
 * @param true 1|0, zda bylo kolo úspěšně či ne
 * @param actualGamer Jméno hráče, který hrál
 * @param fingers Počet zvednutých prstů
 * @param guess Tip
 * @return Vrací zprávu
 */
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

/**
 * Vytváří zprávu o konci hry
 * @param message Ukazatel na zprávu
 * @param winGamer Jméno hráče, který vyhrál
 * @return Vrací zprávu
 */
char * makeENDGAMECommand(char *message, char *winGamer){
    snprintf(message,25,"ENDGAME|%s\n", winGamer);
    return message;
}

/**
 * Vytváří zprávu o tom, že se hráč připojil do hry znovu
 * @param message Ukazatel na zprávu
 * @param whoJoins Jméno hráče
 * @return Vrací zprávu
 */
char * makeReconnectMessageForOthers(char * message, char *whoJoins){
    snprintf(message,35,"RECONNECT|%s\n", whoJoins);
    return message;
}

/**
 * Vytvoří zprávu pro hráče, který se znovu připojuje do hry
 * @param message Ukazatel na zprávu
 * @param game Hra, do které se chce připojit
 * @return Vrací zprávu
 */
char * makeReconnectMessageForRelogPlayer(char * message, Game *game){
    char * hracPlusPrsty = malloc(sizeof(char) * 50);
    char * prsty = malloc(sizeof(char) * 5);
    for (int i = 0; i < game->numOfPlayers; ++i) {
        strcat(hracPlusPrsty,game->gamers[i]->name);
        strcat(hracPlusPrsty,"+");
        sprintf(prsty,"%d|",game->gamers[i]->numOfFingers);
        strcat(hracPlusPrsty,prsty);
    }
    snprintf(message,4*MAX_NAME_LENGTH,"RECONNECT|%d|%s%s\n",game->numOfPlayers,hracPlusPrsty,game->gamers[game->whoPlays]->name);
    free(hracPlusPrsty);
    free(prsty);
    return message;
}
