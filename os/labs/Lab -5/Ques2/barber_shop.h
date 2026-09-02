#ifndef BARBER_SHOP_H
#define BARBER_SHOP_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>

#define PROJ_ID 70
#define MAX_CHAIRS 5

typedef struct {
    int waiting_queue[MAX_CHAIRS];
    int front;
    int rear;
    int count;
    bool terminate;
} BarberShopState;

#endif
