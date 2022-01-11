#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "queue.c"
#include "stateMachine.c"
#include "parser.c"
#include "game.h"
#include "game.c"

//Rozhodovací promměná pro frontu hráčů
int *cislo;
pthread_t thread;
Game **game;
int numOfGames = 0;
int numOfWaitingPlayers = 0;
char *start_message;
int fingerCounter = 0;
int numOfFingersSend = 0;
int guessOfRound = 0;

/**
 * Inicializuje hru
 * @param playersInQueue počet hráčů ve hře
 */
void makeNewGame(int playersInQueue){
    game[numOfGames] = new_game(playersInQueue);
    *cislo = 0;
    for (int i = 0; i < playersInQueue; i++){
        game[numOfGames]->gamers[i] = dequeue();
    }
    game[numOfGames]->numOfPlayers = playersInQueue;
    game[numOfGames]->state = ST_WAITING;
    game[numOfGames]->whoPlays = 0;

    makeSTARTCommand(start_message,playersInQueue,game[numOfGames]);

    for (int i = 0; i < playersInQueue; i++){
        write(game[numOfGames]->gamers[i]->socket,start_message, strlen(start_message));
    }
    numOfWaitingPlayers -= playersInQueue;
    printf("%s", start_message);
    numOfGames++;
}

/**
 * Ukončí hru, pokud ve hře zůstal poslední hráč
 * @param actualGameNumber Číslo hry
 * @param actualGamerNum Číslo hráče
 */
void endGame(int actualGameNumber, int actualGamerNum){
    printf("Hra skončila.\nVyhrál: %s\n",game[actualGameNumber]->gamers[actualGamerNum]->name);
    for (int i = 0; i < game[actualGameNumber]->numOfPlayers; i++){
        free(game[actualGameNumber]->gamers[i]);
    }
    free(game[actualGameNumber]);
    numOfGames--;
    fingerCounter = 0;
    numOfFingersSend = 0;
    guessOfRound = 0;
}

/**
 * Časovač pro čekání na připojení ostatních hráčů v době, kdy jsou do hry připojeni 2 hráči
 * @return
 */
void *timerToNewGame() {
    *cislo = 1;

    sleep(25);

    *cislo = 2;

    makeNewGame(elementsInQueue());
    pthread_exit(cislo);
}

int main (int argc, char **argv) {
    int portNum;

    //Ošetření vstupních portů
    if (argc != 2){
        printf("Bohužel jste zapomněl zadat číslo portu nebo jste zadal až moc parametrů, zkuste to znovu!\n");
        exit(1);
    } else if (strspn(argv[1], "0123456789") != strlen(argv[1])){
        printf("Asi jste omylem zadal do portu i písmenko, zkuste to znovu!\n");
        exit(1);
    } else if ((portNum = atoi(argv[1])) > 65535 || portNum < 0){
        printf("Zadanej port je mimo meze portů, zkuste to znovu!\n");
        exit(1);
    }

    int server_socket;
    int client_socket, fd;
    int return_value;
    Event event;
    cislo = malloc(sizeof(int ));
    *cislo = 0;


    char *errorMessage = "ERROR\n";
    Gamer **gamers = malloc(sizeof(Gamer) * 10);
    int actualGameNumber = 0;
    int actualGamerNum = 0;

    int playersInQueue;

    game = malloc(sizeof(Game) * 10);
    int numOfArgs = 0;
    char **arguments = NULL;
    int flag;

    int nextGamer = 0;
    int numOfGameSomeoneIsTryingToReconnect;

    char welcome_message[] = "Zdárek párek, úspěšně jsi se připojil na server!\n";
    char login_success[] = "Úspěšně jste byl přihlášen, nyní jste ve frontě a prosím počkejte, než vám najdeme protihráče.\n";
    char give_guess[] = "GIVEGUESS\n";
    char *reconnectMessage = malloc(sizeof(char) * MAX_NAME_LENGTH + 12);
    char *reconnectMessageForRelog = malloc(sizeof(char) * MAX_NAME_LENGTH * 2 + 35);
    char *end_game_message = malloc(sizeof(char) * MAX_NAME_LENGTH + 8);
    char *end_of_round_message = malloc(sizeof(char) * 2 * MAX_NAME_LENGTH + 15);
    char **names_ready_to_play = malloc(sizeof(char *) * 4);
    start_message = malloc(sizeof(char) * MAX_NAME_LENGTH * 5 + 15);

    char *input = malloc(sizeof(char) * MAX_INPUT_LENGTH);
    int len_addr;
    int a2read;

    struct sockaddr_in my_addr, peer_addr;
    fd_set client_socks, tests;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket <= 0) return -1;

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(portNum);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    return_value = bind(server_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));

    if (return_value == 0)
        printf("Bind - OK\n");
    else {
        printf("Bind - ERR\n");
        return -1;
    }

    return_value = listen(server_socket, 50);
    if (return_value == 0) {
        printf("Listen - OK\n");
    } else {
        printf("Listen - ER\n");
    }

    // vyprazdnime sadu deskriptoru a vlozime server socket
    FD_ZERO(&client_socks);
    FD_SET(server_socket, &client_socks);

    for (;;) {

        if (*cislo == 2){
            pthread_join(thread,(void**)&cislo);
            goto start;
        }

        tests = client_socks;
        // sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
        return_value = select(FD_SETSIZE, &tests, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

        if (return_value < 0) {
            printf("Select - ERR\n");
            return -1;
        }


        // vynechavame stdin, stdout, stderr
        for (fd = 3; fd < FD_SETSIZE; fd++) {

            if (*cislo == 2){
                pthread_join(thread,(void**)&cislo);
                goto start;
            }

            // je dany socket v sade fd ze kterych lze cist ?
            if (FD_ISSET(fd, &tests)) {
                // je to server socket ? prijmeme nove spojeni
                if (fd == server_socket) {
                    client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
                    FD_SET(client_socket, &client_socks);
                    printf("Pripojen novy klient a pridan do sady socketu %d\n", client_socket);
                    write(client_socket, welcome_message, sizeof(welcome_message) - 1);
                }
                    // je to klientsky socket ? prijmem data
                else {
                    // pocet bajtu co je pripraveno ke cteni
                    ioctl(fd, FIONREAD, &a2read);

                    actualGameNumber = findGameBySocket(fd,numOfGames,game);
                    if (actualGameNumber != -1){
                        actualGamerNum = findGamerBySocket(fd,actualGameNumber,game);
                    }
                    
                    // mame co cist
                    if (a2read > MAX_NAME_LENGTH+7){
                        write(fd,errorMessage, strlen(errorMessage));
                        flag = removePlayerFromGame(findGamerBySocket(fd,actualGameNumber,game),game[actualGameNumber]);
                        if (flag == 1){
                            endGame(actualGameNumber,actualGamerNum);
                        }
                        close(fd);
                        FD_CLR(fd, &client_socks);
                        printf("Klient byl odebrán se sady socketů, moc dlouhá zpráva!\n");
                        if (game[actualGameNumber]->state == ST_GUESSING){
                            goto fingers;
                        }
                        continue;
                    }

                    if (a2read > 0) {

                        memset(input,'\0',MAX_INPUT_LENGTH);

                        if (read(fd, input, a2read) <= 0){
                            printf("Nic nepřišlo");
                        }

                        printf("Prijato: %s", input);

                        arguments = get_args(input,&numOfArgs);

                       

                        event = setEventFromArg(arguments[0]);

                        switch (event) {
                            case EV_LOGIN:
                                if (actualGameNumber == -1){
                                    //Pokus o znovu připojení do hry
                                    numOfGameSomeoneIsTryingToReconnect = findPlayerForRelog(game,arguments[1],numOfGames);
                                    if (numOfGameSomeoneIsTryingToReconnect != -1) {
                                        for (int i = 0; i < game[numOfGameSomeoneIsTryingToReconnect]->numOfPlayers; ++i) {
                                            if (strcmp(game[numOfGameSomeoneIsTryingToReconnect]->gamers[i]->name,arguments[1]) == 0
                                            && game[numOfGameSomeoneIsTryingToReconnect]->gamers[i]->connected == 0){
                                                for (int j = 0; j < game[numOfGameSomeoneIsTryingToReconnect]->numOfPlayers; ++j){
                                                    if (strcmp(game[numOfGameSomeoneIsTryingToReconnect]->gamers[j]->name,arguments[1]) == 0){
                                                        //reconnect daného hráče
                                                        makeReconnectMessageForRelogPlayer(reconnectMessageForRelog,game[numOfGameSomeoneIsTryingToReconnect]);
                                                        game[numOfGameSomeoneIsTryingToReconnect]->gamers[j]->connected = 1;
                                                        game[numOfGameSomeoneIsTryingToReconnect]->gamers[j]->socket = fd;
                                                        write(fd,reconnectMessageForRelog, strlen(reconnectMessageForRelog));
                                                    } else {
                                                        //reconnect zpráva pro ostatní
                                                        makeReconnectMessageForOthers(reconnectMessage,arguments[1]);
                                                        write(game[numOfGameSomeoneIsTryingToReconnect]->gamers[j]->socket,reconnectMessage,
                                                              strlen(reconnectMessage));
                                                    }
                                                }
                                                break;
                                            }
                                        }

                                    //Nikdo se nesnaží připojit do hry
                                    } else {
                                        gamers[numOfWaitingPlayers] = new_gamer();

                                        if (numOfArgs == 1){
                                            strcpy(gamers[numOfWaitingPlayers]->name,"Trouba");
                                        } else {
                                            strcpy(gamers[numOfWaitingPlayers]->name, arguments[1]);
                                        }
                                        gamers[numOfWaitingPlayers]->socket = fd;

                                        enqueue(gamers[numOfWaitingPlayers]);
                                        names_ready_to_play[numOfWaitingPlayers] = gamers[numOfWaitingPlayers]->name;
                                        numOfWaitingPlayers++;

                                        write(fd, login_success, sizeof(login_success) - 1);

                                        start:

                                        playersInQueue = elementsInQueue();
                                        printf("Hráči ve frontě: %d\n", playersInQueue);

                                        if (playersInQueue >= 2 && playersInQueue <= 3) {
                                            //Start countdown
                                            if (*cislo == 0) {
                                                pthread_create(&thread, NULL, &timerToNewGame, NULL);
                                            }
                                        } else if (playersInQueue == 4) {
                                            //Start game
                                            pthread_cancel(thread);
                                            game[numOfGames] = new_game(4);
                                            *cislo = 0;
                                            for (int i = 0; i < 4; i++) {
                                                game[numOfGames]->gamers[i] = dequeue();
                                            }
                                            game[numOfGames]->numOfPlayers = 4;
                                            game[numOfGames]->state = ST_WAITING;
                                            game[numOfGames]->whoPlays = 0;

                                            makeSTARTCommand(start_message, playersInQueue, game[numOfGames]);

                                            for (int i = 0; i < 4; i++) {
                                                write(game[numOfGames]->gamers[i]->socket, start_message,
                                                      strlen(start_message));
                                            }
                                            numOfWaitingPlayers -= 4;
                                            numOfGames++;
                                            break;
                                        }
                                    }

                                } else {
                                    //Hráč, který je již přihlášen se chce znovu přihlásit
                                    write(fd,errorMessage, strlen(errorMessage));
                                    flag = removePlayerFromGame(findGamerBySocket(fd,actualGameNumber,game),game[actualGameNumber]);
                                    if (flag == 1){
                                        endGame(actualGameNumber,actualGamerNum);
                                    }
                                    close(fd);
                                    FD_CLR(fd, &client_socks);
                                    printf("Klient byl odebrán se sady socketů, LOGIN i když je přihlášený!\n");
                                    break;
                                }


                                break;
                            case EV_GUESS:
                                if (allowed_transition(game[actualGameNumber]->state,EV_GUESS) == 0) {
                                    write(fd, errorMessage, strlen(errorMessage));
                                    flag = removePlayerFromGame(findGamerBySocket(fd, actualGameNumber, game),
                                                                game[actualGameNumber]);
                                    if (flag == 1) {
                                        endGame(actualGameNumber, actualGamerNum);
                                    }
                                    close(fd);
                                    FD_CLR(fd, &client_socks);
                                    printf("Klient byl odebrán se sady socketů, GUESS když nemá být GUESS!\n");
                                    break;
                                } else if (actualGamerNum != game[actualGameNumber]->whoPlays) {
                                    write(fd, errorMessage, strlen(errorMessage));
                                    flag = removePlayerFromGame(findGamerBySocket(fd, actualGameNumber, game),
                                                                game[actualGameNumber]);
                                    if (flag == 1) {
                                        endGame(actualGameNumber, actualGamerNum);
                                    }
                                    close(fd);
                                    FD_CLR(fd, &client_socks);
                                    printf("Klient byl odebrán se sady socketů, GUESS, když je na tahu někdo jiný!\n");
                                    break;
                                } else {
                                    if (numOfArgs == 1){
                                        write(fd, errorMessage, strlen(errorMessage));
                                        flag = removePlayerFromGame(findGamerBySocket(fd, actualGameNumber, game),
                                                                    game[actualGameNumber]);
                                        if (flag == 1) {
                                            endGame(actualGameNumber, actualGamerNum);
                                        }
                                        close(fd);
                                        FD_CLR(fd, &client_socks);
                                        printf("Klient byl odebrán se sady socketů, GUESS, bez parametrů!\n");
                                        break;
                                    } else {
                                        guessOfRound = atoi(arguments[1]);
                                    }
                                    game[actualGameNumber]->state = ST_GUESSING;
                                    for (int i = 0; i < game[actualGameNumber]->numOfPlayers; i++){
                                        write(game[actualGameNumber]->gamers[i]->socket,give_guess, strlen(give_guess));
                                    }
                                }

                                break;
                            case EV_FINGERS:
                                if (allowed_transition(game[actualGameNumber]->state,EV_FINGERS) != 0){
                                    if (numOfArgs == 1){
                                        write(fd,errorMessage, strlen(errorMessage));
                                        flag = removePlayerFromGame(findGamerBySocket(fd,actualGameNumber,game),game[actualGameNumber]);
                                        if (flag == 1){
                                            endGame(actualGameNumber,actualGamerNum);
                                        }
                                        close(fd);
                                        FD_CLR(fd, &client_socks);
                                        printf("Klient byl odebrán se sady socketů, FINGERS bez parametrů!\n");
                                        break;
                                    }
                                    if (atoi(arguments[1]) < 0 || atoi(arguments[1]) > game[actualGameNumber]->gamers[findGamerBySocket(fd,actualGameNumber,game)]->numOfFingers){
                                        write(fd,errorMessage, strlen(errorMessage));
                                        flag = removePlayerFromGame(findGamerBySocket(fd,actualGameNumber,game),game[actualGameNumber]);
                                        if (flag == 1){
                                            endGame(actualGameNumber,actualGamerNum);
                                        }
                                        close(fd);
                                        FD_CLR(fd, &client_socks);
                                        printf("Klient byl odebrán se sady socketů, FINGERS nevalidní hodnota!\n");
                                        break;
                                    }
                                    if (2 == 1){
                                        fingers:
                                        fingerCounter += 0;
                                    } else {
                                        fingerCounter += atoi(arguments[1]);
                                        numOfFingersSend++;
                                    }
                                    if (numOfFingersSend == numOfActivePlayers(game[actualGameNumber])){
                                        if (fingerCounter == guessOfRound){
                                            if (game[actualGameNumber]->gamers[game[actualGameNumber]->whoPlays]->numOfFingers == 1){
                                                makeENDGAMECommand(end_game_message,game[actualGameNumber]->gamers[game[actualGameNumber]->whoPlays]->name);
                                                for (int i = 0; i < game[actualGameNumber]->numOfPlayers; i++){
                                                    write(game[actualGameNumber]->gamers[i]->socket,end_game_message, strlen(end_game_message));
                                                }
                                                printf("Hra skončila.\nVyhrál: %s\n",game[actualGameNumber]->gamers[actualGamerNum]->name);
                                                for (int i = 0; i < game[actualGameNumber]->numOfPlayers; i++){
                                                    free(game[actualGameNumber]->gamers[i]);
                                                }
                                                free(game[actualGameNumber]);
                                                numOfGames--;
                                                fingerCounter = 0;
                                                numOfFingersSend = 0;
                                                guessOfRound = 0;
                                                break;
                                            } else {
                                                nextGamer = nextPlayer(game[actualGameNumber],game[actualGameNumber]->whoPlays);
                                                makeENDOFROUNDCommand(end_of_round_message,game[actualGameNumber]->gamers[nextGamer]->name,1,game[actualGameNumber]->gamers[game[actualGameNumber]->whoPlays]->name,fingerCounter,guessOfRound);
                                                for (int i = 0; i < game[actualGameNumber]->numOfPlayers; i++){
                                                    write(game[actualGameNumber]->gamers[i]->socket,end_of_round_message, strlen(end_of_round_message));
                                                }
                                                game[actualGameNumber]->gamers[game[actualGameNumber]->whoPlays]->numOfFingers--;
                                                game[actualGameNumber]->whoPlays = nextGamer;
                                                game[actualGameNumber]->state = ST_WAITING;
                                            }
                                        } else {
                                            nextGamer = nextPlayer(game[actualGameNumber],game[actualGameNumber]->whoPlays);
                                            makeENDOFROUNDCommand(end_of_round_message,game[actualGameNumber]->gamers[nextGamer]->name,0,game[actualGameNumber]->gamers[game[actualGameNumber]->whoPlays]->name,fingerCounter,guessOfRound);
                                            for (int i = 0; i < game[actualGameNumber]->numOfPlayers; i++){
                                                write(game[actualGameNumber]->gamers[i]->socket,end_of_round_message, strlen(end_of_round_message));
                                            }
                                            game[actualGameNumber]->whoPlays = nextGamer;
                                            game[actualGameNumber]->state = ST_WAITING;
                                        }
                                        fingerCounter = 0;
                                        numOfFingersSend = 0;
                                        guessOfRound = 0;
                                    }
                                } else {
                                    write(fd,errorMessage, strlen(errorMessage));
                                    flag = removePlayerFromGame(findGamerBySocket(fd,actualGameNumber,game),game[actualGameNumber]);
                                    if (flag == 1){
                                        endGame(actualGameNumber,actualGamerNum);
                                    }
                                    close(fd);
                                    FD_CLR(fd, &client_socks);
                                    printf("Klient byl odebrán se sady socketů, FINGERS když nemají přijít!\n");
                                    break;
                                }
                                break;
                            case EV_WRONG:
                                write(fd,errorMessage, strlen(errorMessage));
                                if (actualGameNumber != -1){
                                    flag = removePlayerFromGame(findGamerBySocket(fd,actualGameNumber,game),game[actualGameNumber]);
                                    if (flag == 1){
                                        endGame(actualGameNumber,actualGamerNum);
                                    }
                                }
                                close(fd);
                                FD_CLR(fd, &client_socks);
                                printf("Klient byl odebrán se sady socketů, píše blbosti!\n");
                                break;
                        }

                    }
                        // na socketu se stalo neco spatneho
                    else {
                        flag = 0;
                        int actualGame = findGameBySocket(fd,numOfGames,game);
                        if (actualGame != -1){
                            int gamerLeft = findGamerBySocket(fd,actualGame,game);
                            flag = removePlayerFromGame(gamerLeft,game[actualGame]);
                        }
                        if (flag == 1){
                            endGame(actualGameNumber,actualGamerNum);
                        }
                        close(fd);
                        FD_CLR(fd, &client_socks);
                        printf("Klient se odpojil a byl odebran ze sady socketů\n");
                    }
                }
            }
        }

    }

    for (int i = 0; i < sizeof(gamers); ++i) {
        free(gamers[i]);
    }
    free(gamers);
    for (int i = 0; i < sizeof(game); ++i) {
        free(game[i]);
    }
    free(game);
    for (int i = 0; i < sizeof(names_ready_to_play);i++){
        free(names_ready_to_play[i]);
    }
    free(names_ready_to_play);
    free(start_message);
    free(end_of_round_message);
    free(end_game_message);
    free(cislo);
    free(input);
    return 0;
}
