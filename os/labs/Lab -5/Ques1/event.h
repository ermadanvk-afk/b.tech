#ifndef EVENT_H
#define EVENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>

#define PROJ_ID 65
#define MAX_STR_LEN 256

typedef struct {
    int event_id;
    char event_type[MAX_STR_LEN];
    char event_message[MAX_STR_LEN];
    bool is_new;
    bool terminate;
} EventData;

#endif
