#include "barber_shop.h"

int main() {
    key_t key = ftok("barber.c", PROJ_ID);
    if (key == -1) {
        perror("ftok failed. Make sure barber.c exists.");
        exit(1);
    }

    int shmid = shmget(key, sizeof(BarberShopState), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    BarberShopState *shop = (BarberShopState *)shmat(shmid, NULL, 0);
    if (shop == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }
    
    while(1) {
        int cust_id;
        printf("Enter Customer ID (or -1 to stop): ");
        if (scanf("%d", &cust_id) != 1) break;
        
        if (cust_id == -1) {
            shop->terminate = true;
            break;
        }
        
        if (shop->count < MAX_CHAIRS) {
            shop->waiting_queue[shop->rear] = cust_id;
            shop->rear = (shop->rear + 1) % MAX_CHAIRS;
            shop->count++;
            printf("Customer %d is waiting in a chair.\n", cust_id);
        } else {
            printf("Customer %d leaves (no chairs available).\n", cust_id);
        }
        usleep(500000);
    }
    
    shmdt(shop);
    return 0;
}
