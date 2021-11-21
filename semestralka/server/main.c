#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
// kvuli iotctl
#include <sys/ioctl.h>
#include <signal.h>

#include "queue.c"
#include "stateMachine.c"
#include "parser.c"
#include "game.h"


#define MAX_INPUT_LENGTH 75

Gamer *new_gamer(){
    Gamer *gamer1;
    gamer1 = malloc(sizeof(Gamer));
    return gamer1;
}

Game *new_game(int numOfPlaye){
    Game *gameska;
    gameska = malloc(sizeof(Game));
    for (int i = 0; i <numOfPlaye;i++){
        gameska->gamers[i] = new_gamer();
    }
    gameska->state = ST_INIT;
    return gameska;
}

int main (void) {

    int server_socket;
    int client_socket, fd;
    int return_value;
    Event event;
    //Rozhodovací promměná pro frontu hráčů
    int decide = 0;
    //Proměnná pro zabití dítěte
    int id = -42;

    Gamer **gamers = malloc(sizeof(Gamer) * 10);
    int numOfWaitingPlayers = 0;
    int playersInQueue;
    Game *game = NULL;
    int numOfArgs = 0;
    char **arguments = NULL;

    char welcome_message[] = "Zdárek párek, úspěšně jsi se připojil na server!\n";
    char login_success[] = "Úspěšně jste byl přihlášen, nyní jste ve frontě a prosím počkejte, než vám najdeme protihráče.\n";
    char **names_ready_to_play = malloc(sizeof(char *) * 4);
    char *start_message = malloc(sizeof(char) * MAX_NAME_LENGTH * 5);

    char *input = malloc(sizeof(char) * MAX_INPUT_LENGTH);
    int len_addr;
    int a2read;

    struct sockaddr_in my_addr, peer_addr;
    fd_set client_socks, tests;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket <= 0) return -1;

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(10000);
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

        tests = client_socks;
        // sada deskriptoru je po kazdem volani select prepsana sadou deskriptoru kde se neco delo
        return_value = select(FD_SETSIZE, &tests, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

        if (return_value < 0) {
            printf("Select - ERR\n");
            return -1;
        }
        // vynechavame stdin, stdout, stderr
        for (fd = 3; fd < FD_SETSIZE; fd++) {

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
                    //printf("ne - %s\n", response);
                    // pocet bajtu co je pripraveno ke cteni
                    ioctl(fd, FIONREAD, &a2read);
                    // mame co cist

                    if (a2read > 75){
                        printf("Moc dlouhá zpráva, čau");
                        return -1;
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
                                gamers[numOfWaitingPlayers] = new_gamer();

                                gamers[numOfWaitingPlayers]->numOfFingers = 2;
                                strcpy(gamers[numOfWaitingPlayers]->name,arguments[1]);
                                gamers[numOfWaitingPlayers]->socket = fd;

                                enqueue(gamers[numOfWaitingPlayers]);
                                names_ready_to_play[numOfWaitingPlayers] = gamers[numOfWaitingPlayers]->name;
                                numOfWaitingPlayers++;

                                gamers[numOfWaitingPlayers] = new_gamer();

                                gamers[numOfWaitingPlayers]->numOfFingers = 2;
                                strcpy(gamers[numOfWaitingPlayers]->name,arguments[1]);
                                gamers[numOfWaitingPlayers]->socket = fd;

                                enqueue(gamers[numOfWaitingPlayers]);
                                names_ready_to_play[numOfWaitingPlayers] = gamers[numOfWaitingPlayers]->name;
                                numOfWaitingPlayers++;
                                gamers[numOfWaitingPlayers] = new_gamer();

                                gamers[numOfWaitingPlayers]->numOfFingers = 2;
                                strcpy(gamers[numOfWaitingPlayers]->name,arguments[1]);
                                gamers[numOfWaitingPlayers]->socket = fd;

                                enqueue(gamers[numOfWaitingPlayers]);
                                names_ready_to_play[numOfWaitingPlayers] = gamers[numOfWaitingPlayers]->name;
                                numOfWaitingPlayers++;
                                gamers[numOfWaitingPlayers] = new_gamer();


                                playersInQueue = elementsInQueue();
                                printf("%d\n",playersInQueue);
                                strcpy(start_message,"START|");

                                if (playersInQueue >= 2 && playersInQueue <= 3){
                                    //Start countdown
                                    printf("Jsem tu\n");
                                    start:
                                    if (decide == 0) {
                                        id = fork();
                                        if (id == 0) {
                                            decide = 1;
                                            sleep(15);
                                            decide = 2;
                                            printf("Nespim\n");
                                            goto start;
                                        }
                                    } else if (decide == 2) {
                                        kill(0,SIGKILL);
                                        playersInQueue = elementsInQueue();
                                        game = new_game(playersInQueue);
                                        for (int i = 0; i < playersInQueue; i++){
                                            game->gamers[i] = dequeue();
                                            strcat(start_message,game->gamers[i]->name);
                                            if (i != playersInQueue-1){
                                                strcat(start_message,"|");
                                            }
                                        }
                                        printf("%s\n",start_message);
                                    }


                                    //dequeue()
                                } else if (playersInQueue == 4){
                                    //Start game
                                    char *neco = "4|IDHRÁČECOZAČÍNÁ|";
                                    game = new_game(4);
                                    strcat(start_message,neco);
                                    for (int i = 0; i < 4; i++){
                                        game->gamers[i] = dequeue();
                                        strcat(start_message,game->gamers[i]->name);
                                        if (i != 3){
                                            strcat(start_message,"|");
                                        }
                                    }
                                    /*
                                    for (int i = 0; i < 4; i++){
                                        write(game->gamers[i]->socket,start_message, sizeof(start_message) - 1);
                                    }
                                     */
                                    neco = "\n";
                                    strcat(start_message,neco);
                                    write(game->gamers[0]->socket,start_message, strlen(start_message));

                                }

                                write(client_socket, login_success, sizeof(login_success) - 1);
                                sleep(3);
                                write(client_socket, login_success, sizeof(login_success) - 1);
                                break;
                            case EV_GUESS:
                                //
                                break;
                            case EV_FINGERS:
                                //
                                break;
                            case EV_WRONG:
                                close(fd);
                                FD_CLR(fd, &client_socks);
                                printf("Klient byl odebrán se sady socketů, píše blbosti!");
                                break;
                        }

                    }
                        // na socketu se stalo neco spatneho
                    else {
                        close(fd);
                        FD_CLR(fd, &client_socks);
                        printf("Klient se odpojil a byl odebran ze sady socketů\n");
                    }
                }
            }
        }

    }
    free(input);
    return 0;
}