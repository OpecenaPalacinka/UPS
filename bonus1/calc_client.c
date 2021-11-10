#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>

int calc(char *operand, int firstNum, int secNum){
    int result;

    if (strcmp(operand,"plus") == 0){
        result = firstNum + secNum;
    } else if (strcmp(operand,"minus") == 0){
        result = firstNum - secNum;
    } else if (strcmp(operand, "multiply") == 0){
        result = firstNum * secNum;
    } else if (strcmp(operand, "division") == 0){
        if (secNum != 0)
            result = firstNum / secNum;
        else {
            printf("Division by zero! Result is 0");
            result = 0;
        }
    } else {
        printf("Wrong operand! Your result is set to 0!\n");
        result = 0;
    }

    return result;
}

int main(void)
{
    int client_sock;
    int addr_len;
    struct sockaddr_in remote_addr;
    int return_value, result;
    char message[255], input[255], input_to_strtok[255];

    client_sock = socket(AF_INET, SOCK_STREAM, 0 );

    memset(&remote_addr, 0, sizeof(struct sockaddr_in));

    if (client_sock<=0){
        return -1;
    }

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("34.122.204.187");
    remote_addr.sin_port = htons(2000);
    addr_len=sizeof( remote_addr );

    return_value = connect(client_sock, (struct sockaddr *)&remote_addr,\
		 sizeof(struct sockaddr_in));
    if (return_value == 0)
        printf("Connect - OK\n");
    else {
        printf("Connect - ERR\n");
        return -1;
    }

    if( recv( client_sock, message, 255,0) < 0)
    {
        printf("Recv failed\n");
        return -1;
    }

    printf("%s",message);

    if( recv( client_sock, message, 255,0) < 0)
    {
        printf("Recv failed\n");
        return -1;
    }

    printf("%s",message);

    fgets(input,255,stdin);

    strcpy(input_to_strtok,input);

    char *operand = strtok(input_to_strtok,"|");
    char *firstNum = strtok(NULL, "|");
    char *secNum = strtok(NULL,"|");

    result = calc(operand,atoi(firstNum),atoi(secNum));

    input[strlen(input)] = '\n';

    if( send(client_sock , &input , 255 , 0) < 0)
    {
        printf("Send failed\n");
        return -1;
    }

    memset(message,0,255);

    if( recv( client_sock, message, 255,0) < 0)
    {
        printf("Recv failed\n");
        return -1;
    }

    printf("Dostavam = %s\n",  message);

    char *reaction = strtok(message,"|");
    char *num = strtok(NULL,"|");

    printf("Muj vysledek -> %d\n",result);

    if (atoi(num) == result && strcmp(reaction,"OK") == 0){
        printf("Vysledek vyhodnocen spravne!\n");
    } else if (atoi(num) == result && strcmp(reaction,"ERROR") == 0){
        printf("Vysledek je spravne ale spatna odpoved!\n");
    } else if (atoi(num) != result && strcmp(reaction,"OK")){
        printf("Vysledky se nerovnaji ale byly vyhodnoceny jako OK\n");
    } else {
        printf("Chybka\n");
    }

    close(client_sock);
    return 0 ;
}