#include "event.h"

int main() {
    key_t key = ftok("event_generator.c", PROJ_ID);
    if (key == -1) {
        perror("ftok failed");
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

    shared_data->is_new = false;
    shared_data->terminate = false;
    
    int id_counter = 1;

    while (1) {
        char type[MAX_STR_LEN];
        char message[MAX_STR_LEN];

        printf("Enter Event Type (or 'exit' to terminate): ");
        if (fgets(type, sizeof(type), stdin) == NULL) break;
        type[strcspn(type, "\n")] = 0; // Remove newline

        if (strcmp(type, "exit") == 0) {
            shared_data->terminate = true;
            break;
        }

        printf("Enter Event Message: ");
        if (fgets(message, sizeof(message), stdin) == NULL) break;
        message[strcspn(message, "\n")] = 0;

        while (shared_data->is_new) {
            usleep(100000); // Wait for monitor to process
        }

        shared_data->event_id = id_counter++;
        strncpy(shared_data->event_type, type, MAX_STR_LEN);
        strncpy(shared_data->event_message, message, MAX_STR_LEN);
        
        shared_data->is_new = true;
        printf("Event %d generated successfully.\n\n", shared_data->event_id);
    }

    shmdt(shared_data);
    printf("Event Generator terminated.\n");
    return 0;
}