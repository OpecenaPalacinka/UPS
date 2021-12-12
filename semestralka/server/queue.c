#include <stdio.h>
#include <stdlib.h>

#include "game.h"

# define SIZE 100

void enqueue(Gamer *gamer);
Gamer *dequeue();
int elementsInQueue();

Gamer *inp_arr[SIZE];
int Rear = - 1;
int Front = - 1;

void enqueue(Gamer *gamer)
{
    if (Rear == SIZE - 1)
        printf("Overflow \n");
    else
    {
        if (Front == - 1){
            Front = 0;
        }
        printf("Gamer inserted.\n");
        Rear++;
        inp_arr[Rear] = gamer;
    }
}

Gamer *dequeue()
{
    if (Front == - 1 || Front > Rear)
    {
        printf("Underflow \n");
        return (Gamer*) NULL;
    }
    else
    {
        printf("Element deleted \n");
        Front++;
        return inp_arr[Front-1];
    }
}

int elementsInQueue(){
    return Rear - Front + 1;
}


