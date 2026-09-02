#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_MSG 256

int main() {
    int pipe1[2]; // Parent to Child
    int pipe2[2]; // Child to Parent
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe failed");
        exit(1);
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid > 0) { // Parent
        close(pipe1[0]); // Close reading end of pipe 1
        close(pipe2[1]); // Close writing end of pipe 2
        
        char message[MAX_MSG];
        char ack[MAX_MSG];
        
        while (1) {
            printf("Parent: Enter message (or 'exit' to terminate): ");
            if (fgets(message, sizeof(message), stdin) == NULL) break;
            message[strcspn(message, "\n")] = 0;
            
            write(pipe1[1], message, strlen(message) + 1);
            
            if (strcmp(message, "exit") == 0) {
                break;
            }
            
            read(pipe2[0], ack, sizeof(ack));
            printf("Parent: Acknowledgement from child: %s\n\n", ack);
        }
        
        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    } else { // Child
        close(pipe1[1]); // Close writing end of pipe 1
        close(pipe2[0]); // Close reading end of pipe 2
        
        char message[MAX_MSG];
        
        while (1) {
            if (read(pipe1[0], message, sizeof(message)) > 0) {
                if (strcmp(message, "exit") == 0) {
                    break;
                }
                
                printf("Child: Received message: %s\n", message);
                
                char ack[] = "Message received successfully.";
                write(pipe2[1], ack, strlen(ack) + 1);
            }
        }
        
        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }
    
    return 0;
}
