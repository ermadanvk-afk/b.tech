#include "event.h"

int main() {
    key_t key = ftok("event_generator.c", PROJ_ID);
    if (key == -1) {
        perror("ftok failed. Make sure event_generator.c exists.");
        exit(1);
    }

    int shmid = shmget(key, sizeof(EventData), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    EventData *shared_data = (EventData *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    printf("Event Monitor started. Waiting for events...\n\n");

    while (1) {
        if (shared_data->is_new) {
            printf("--- New Event Received ---\n");
            printf("Event ID: %d\n", shared_data->event_id);
            printf("Event Type: %s\n", shared_data->event_type);
            printf("Event Message: %s\n", shared_data->event_message);
            printf("--------------------------\n\n");
            
            shared_data->is_new = false; 
        }
        
        if (shared_data->terminate && !shared_data->is_new) {
            break;
        }
        
        usleep(100000);
    }

    shmdt(shared_data);
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl RMID failed");
    }

    printf("Event Monitor terminated and shared memory cleaned up.\n");
    return 0;
}
