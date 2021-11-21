//============================================================================
// Name        : Server.cpp
// Author      : Viktor Cerny
// Version     :
// Copyright   : Your copyright notice
// Description : TCP server - demo
//============================================================================

#include <iostream>
using namespace std;

#include <cstdlib>
#include <cstdio>
#include <sys/socket.h> // socket(), bind(), connect(), listen()
#include <unistd.h> // close(), read(), write()
#include <netinet/in.h> // struct sockaddr_in
#include <strings.h> // bzero()
#include <wait.h> // waitpid()

#define BUFFER_SIZE 10240
#define TIMEOUT 10


int main(int argc, char **argv) {

    if(argc < 2){
        cerr << "Usage: " << argv[0] << " <port>" << endl;
        return -1;
    }

    // Vytvoreni koncoveho bodu spojeni
    int l = socket(AF_INET, SOCK_STREAM, 0);
    if(l < 0){
        perror("Nemohu vytvorit socket: ");
        return -1;
    }

    int port = atoi(argv[1]);
    if(port == 0){
        cerr << "Usage: server port" << endl;
        close(l);
        return -1;
    }

    struct sockaddr_in adresa;
    bzero(&adresa, sizeof(adresa));
    adresa.sin_family = AF_INET;
    adresa.sin_port = htons(port);
    adresa.sin_addr.s_addr = htonl(INADDR_ANY);

    // Prirazeni socketu k rozhranim
    if(bind(l, (struct sockaddr *) &adresa, sizeof(adresa)) < 0){
        perror("Problem s bind(): ");
        close(l);
        return -1;
    }

    // Oznacim socket jako pasivni
    if(listen(l, 10) < 0){
        perror("Problem s listen()!");
        close(l);
        return -1;
    }

    struct sockaddr_in vzdalena_adresa;
    socklen_t velikost;

    while(true){
        // Cekam na prichozi spojeni
        int c = accept(l, (struct sockaddr *) &vzdalena_adresa, &velikost);
        if(c < 0){
            perror("Problem s accept()!");
            close(l);
            return -1;
        }
        pid_t pid = fork();
        if(pid == 0){
            // Kopie hlavniho vlakna ma vlastni referenci na naslouchajici soket.
            // Podproces, ktery obsluhuje klienta, tuto referenci nepotrebuje, takze je dobre
            // tuto referenci smazat. V hlavnim vlakne samozrejme reference na naslouchajici
            // soket zustava.
            close(l);

            struct timeval timeout;
            timeout.tv_sec = TIMEOUT;
            timeout.tv_usec = 0;

            fd_set sockets;

            int retval;
            char buffer[BUFFER_SIZE];
            while(true){
                FD_ZERO(&sockets);
                FD_SET(c, &sockets);
                // Prvnim parametrem je cislo nejvyssiho soketu v 'sockets' zvysene o jedna.
                // (Velka jednoduchost a efektivvnost funkce je vyvazena spatnou
                // ergonomii pro uzivatele.)
                // Posledni z parametru je samotny timeout. Je to struktura typu 'struct timeval',
                // ktera umoznuje casovani s presnosti na mikrosekundy (+/-). Funkci se predava
                // odkaz na promennou a funkce ji muze modifikovat. Ve vetsine implementaci
                // odecita funkce od hodnoty timeoutu cas straveny cekanim. Podle manualu na
                // to nelze spolehat na vsech platformach a ve vsech implementacich funkce
                // select()!!!
                retval = select(c + 1, &sockets, NULL, NULL, &timeout);
                if(retval < 0){
                    perror("Chyba v select(): ");
                    close(c);
                    return -1;
                }
                if(!FD_ISSET(c, &sockets)){
                    // Zde je jasne, ze funkce select() skoncila cekani kvuli timeoutu.
                    cout << "Connection timeout!" << endl;
                    close(c);
                    return 0;
                }
                int bytesRead = recv(c, buffer, BUFFER_SIZE - 1, 0);
                if(bytesRead <= 0){
                    perror("Chyba pri cteni ze socketu: ");
                    close(c);
                    return -3;
                }
                buffer[bytesRead] = '\0';
                // Pokud prijmu "konec" ukoncim aktualni dceriny proces
                if(string("konec") == buffer){
                    break;
                }
                cout << buffer << endl;
            }
            close(c);
            return 0;
        }

        // Aby nam nezustavaly v systemu zombie procesy po kazdem obslouzeneme klientovi,
        // je nutne otestovat, zda se uz nejaky podproces ukoncil.
        // Prvni argument rika, cekej na jakykoliv proces potomka, treti argument zajisti,
        // ze je funkce neblokujici (standardne blokujici je, coz ted opravdu nechceme).
        int status = 0;
        waitpid(0, &status, WNOHANG);

        close(c); // Nove vytvoreny socket je nutne zavrit v hlavnim procesu, protoze by na nem v systemu
        // zustala reference a jeho zavreni v novem procesu by nemelo zadny efekt.
    }

    close(l);
    return 0;
}
