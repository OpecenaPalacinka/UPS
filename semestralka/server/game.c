//
// Created by Pelikán on 17.11.2021.
//

#include <stdio.h>
#include <string.h>
#include <malloc.h>

/**
 * Najde gamesku podle čísla socketu hráče
 * Používá se, když někdo pošle command, aby se zjistilo, o jakou hru se jedná
 * @param socket Číslo socketu
 * @param totalNumOfGames Počet her
 * @param game Všechny hry
 * @return Index hry
 */
int findGameBySocket(int socket, int totalNumOfGames, Game **game){
    int numOfGame = -1;
    if (totalNumOfGames == 0){
        return -1;
    }
    for (int i = 0; i < totalNumOfGames; ++i) {
        for (int j = 0; j < game[i]->numOfPlayers; ++j) {
            if (game[i]->gamers[j]->connected == 1){
                if (game[i]->gamers[j]->socket == socket){
                    numOfGame = i;
                    return numOfGame;
                }
            }
        }
    }
    return numOfGame;
}

/**
 * Najde hráče podle čísla socketu
 * Používá se, když někdo pošle command, aby se zjistilo, kdo ho poslal
 * @param socket číslo socketu
 * @param actualGame číslo hry
 * @param game Všechny hry
 * @return Index hráče
 */
int findGamerBySocket(int socket, int actualGame, Game **game){
    int numOfGamer = -1;
    for (int j = 0; j < game[actualGame]->numOfPlayers; ++j) {
        if (game[actualGame]->gamers[j]->connected == 1){
            if (game[actualGame]->gamers[j]->socket == socket){
                numOfGamer = j;
                return numOfGamer;
            }
        }
    }
    return numOfGamer;
}

/**
 * Zjištění, kdo je další na řadě
 * @param game Hra
 * @param actualGamer Aktuálně na řadě
 * @return Index hráče
 */
int nextPlayer(Game *game, int actualGamer){

    if (game->numOfPlayers - 1 >= actualGamer + 1){
        if (game->gamers[actualGamer+1]->connected == 0){
            nextPlayer(game,actualGamer+1);
        } else {
            printf("%d + 1\n",actualGamer);
            return actualGamer + 1;
        }
    } else {
        if (game->gamers[0]->connected == 0){
            nextPlayer(game,0);
        } else {
            printf("0\n");
            return 0;
        }
    }
}

/**
 * Při odpojení hráče, rozešle zprávy, a odpojí hráče
 * @param playerLeft Index hráče
 * @param game Hra
 * @return 1- hra skončila, 0- hra pokračuje
 */
int removePlayerFromGame(int playerLeft, Game *game){
    char *message = malloc(sizeof(char)*50);
    char *name = game->gamers[playerLeft]->name;
    game->gamers[playerLeft]->connected = 0;

    if (game->numOfPlayers == 2){
        if (playerLeft == 0){
            makeENDGAMECommand(message,game->gamers[1]->name);
        } else {
            makeENDGAMECommand(message,game->gamers[0]->name);
        }
        for (int i = 0; i < game->numOfPlayers; ++i) {
            write(game->gamers[i]->socket,message, strlen(message));
        }
        return 1;
    } else {
        snprintf(message,50,"LOGOUT|%s|%s\n",name, game->gamers[nextPlayer(game, game->whoPlays-1)]->name);
        game->whoPlays = nextPlayer(game, game->whoPlays-1);
        for (int i = 0; i < game->numOfPlayers; ++i) {
            write(game->gamers[i]->socket,message, strlen(message));
        }
        return 0;
    }
}

/**
 * Najde, zda se hráč s jménem "nameToFind" chce připojit do hry, nebo je to nový hráč
 * @param game Všechny hry
 * @param nameToFind Jméno hráče
 * @param totalNumOfGames Počet her
 * @return -1 pokud se tento hráč nechce znovu připojit nebo neexistuje, jinak vrátí číslo hry
 */
int findPlayerForRelog(Game **game, char *nameToFind,int totalNumOfGames){
    int numOfGame = -1;
    for (int i = 0; i < totalNumOfGames; ++i) {
        for (int j = 0; j < game[i]->numOfPlayers; ++j) {
            if (strcmp(game[i]->gamers[j]->name,nameToFind) == 0 && game[i]->gamers[j]->connected == 0){
                numOfGame = i;
                return numOfGame;
            }
        }
    }
    return numOfGame;
}

/**
 * Spočítá aktuální počet aktivních hráčů ve hře
 * @param game Hra
 * @return Počet hráčů
 */
int numOfActivePlayers(Game *game){
    int num = 0;
    for (int i = 0; i < game->numOfPlayers; ++i) {
        if (game->gamers[i]->connected == 1){
            num++;
        }
    }
    return num;
}

/**
 * Funkce vytvoří nového hráče
 * @return vrací hráče
 */
Gamer *new_gamer(){
    Gamer *gamer1;
    gamer1 = malloc(sizeof(Gamer));
    gamer1->numOfFingers = 2;
    gamer1->connected = 1;
    return gamer1;
}

/**
 * Alokuje místa v paměti pro hru a hráče
 * @param numOfPlaye Počet hráčů ve hře
 * @return vrací hru
 */
Game *new_game(int numOfPlaye){
    Game *gameska;
    gameska = malloc(sizeof(Game));
    for (int i = 0; i <numOfPlaye;i++){
        gameska->gamers[i] = new_gamer();
    }
    gameska->state = ST_INIT;
    return gameska;
}

