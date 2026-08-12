#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("I am child. PID = %d\n", getpid());
    } else {
        // Parent process
        printf("I am parent. PID = %d\n", getpid());
    }

    return 0;
}