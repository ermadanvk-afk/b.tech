#include "barber_shop.h"

int main() {
    key_t key = ftok("barber.c", PROJ_ID);
    if (key == -1) {
        perror("ftok failed");
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
    
    shop->front = 0;
    shop->rear = 0;
    shop->count = 0;
    shop->terminate = false;
    
    printf("Barber Shop is Open.\n");
    
    while(1) {
        if (shop->count == 0) {
            if (shop->terminate) {
                break;
            }
            printf("Barber is sleeping... Zzz\n");
            while (shop->count == 0 && !shop->terminate) {
                usleep(500000);
            }
        }
        
        if (shop->count > 0) {
            int cust_id = shop->waiting_queue[shop->front];
            shop->front = (shop->front + 1) % MAX_CHAIRS;
            shop->count--;
            
            printf("Barber is serving Customer %d.\n", cust_id);
            sleep(2); 
            printf("Barber finished serving Customer %d.\n", cust_id);
        }
    }
    
    printf("Barber Shop is closing.\n");
    
    shmdt(shop);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
