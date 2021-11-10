#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void)
{
    int client_sock;
    int addr_len;
    struct sockaddr_in remote_addr;
    int return_value;
    char message[255], input[255];

    client_sock = socket(AF_INET, SOCK_STREAM, 0 );

    memset(&remote_addr, 0, sizeof(struct sockaddr_in));

    if (client_sock<=0){
        return -1;
    }

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("34.122.204.187");
    remote_addr.sin_port = htons(2001);
    addr_len=sizeof( remote_addr );

    return_value = connect(client_sock, (struct sockaddr *)&remote_addr,\
		 sizeof(struct sockaddr_in));
    if (return_value == 0)
        printf("Connect - OK\n");
    else {
        printf("Connect - ERR\n");
        return -1;
    }

    fgets(input,255,stdin);

    if( send(client_sock , input , strlen(input) , 0) < 0)
    {
        printf("Send failed\n");
        return -1;
    }

    if( recv( client_sock, message, 255,0) < 0)
    {
        printf("Recv failed\n");
        return -1;
    }

    printf( "Dostavam = %s\n",  message);

    if (strcmp(message,input) == 0){
        printf("Vložená zpráva došla správně z5!\n");
    } else {
        printf("Vložená zpráva přišla špatně!\n");
    }

    close(client_sock);
    return 0 ;
}