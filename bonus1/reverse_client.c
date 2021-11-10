#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>

void reverse(char *str)
{
    if (str != 0 && *str != '\0') // Non-null pointer; non-empty string
    {
        char *end = str + strlen(str) - 1;

        while (str < end)
        {
            char tmp = *str;
            *str++ = *end;
            *end-- = tmp;
        }
    }
}

int main(void)
{
    int client_sock;
    int addr_len;
    struct sockaddr_in remote_addr;
    int return_value, i, j = 0, counter = 0;
    char message[255], answer[255];

    client_sock = socket(AF_INET, SOCK_STREAM, 0 );

    memset(&remote_addr, 0, sizeof(struct sockaddr_in));

    if (client_sock<=0){
        return -1;
    }

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("34.122.204.187");
    remote_addr.sin_port = htons(2002);
    addr_len=sizeof( remote_addr );

    return_value = connect(client_sock, (struct sockaddr *)&remote_addr,\
		 sizeof(struct sockaddr_in));
    if (return_value == 0)
        printf("Connect - OK\n");
    else {
        printf("Connect - ERR\n");
        return -1;
    }

    memset(message,0,255);

    if( recv( client_sock, message, 255,0) < 0)
    {
        printf("Recv failed\n");
        return -1;
    }

    printf( "Dostavam = %s\n",  message);

    reverse(message);
    strcat(message,"\n");

    printf("Otočený = %s\n",message);

    int result = send(client_sock , &message , strlen(message) , 0);

    if( result < 0)
    {
        printf("Send failed\n");
        return -1;
    }

    while (42){
        if( recv( client_sock, answer, 255,0) > 2)
        {
            printf("%s",answer);
            if (strcmp(strtok(answer,"\n"),"OK")==0){
                close(client_sock);
                return 0;
            } else {
                printf("Nepovedlo se!\n");
                close(client_sock);
                return -1;
            }
        }

    }

    close(client_sock);
    return 0 ;
}