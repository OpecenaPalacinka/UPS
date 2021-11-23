//
// Created by Pelikán on 17.11.2021.
//

#include <string.h>
#include <malloc.h>

#define DELIM "|"

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

Event setEventFromArg(char *event){
    if (strcmp("LOGIN",event) == 0){
        return EV_LOGIN;
    } else if (strcmp("GUESS",event) == 0){
        return EV_GUESS;
    } else if (strcmp("FINGERS",event) == 0){
        return EV_FINGERS;
    } else {
        printf("Wrong input! ENDING");
        return EV_WRONG;
    }
}

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
