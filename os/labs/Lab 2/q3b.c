#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // Step 1: Check command-line arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <command> <argument>\n", argv[0]);
        exit(1);
    }

    // Step 2: Create a child process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Step 3: Child process — execute the requested command
        execlp(argv[1], argv[1], argv[2], NULL);
        
        // If execlp returns, execution failed
        perror("execlp failed");
        exit(1);
    } else {
        // Step 4: Parent process — wait for the child
        int status;
        pid_t waited_pid = waitpid(pid, &status, 0);
        
        if (waited_pid == -1) {
            perror("waitpid failed");
            exit(1);
        }
        
        if (WIFEXITED(status)) {
            printf("Child process exited successfully with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process terminated abnormally\n");
        }
    }

    return 0;
}