#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64
#define MAX_BG_PROCS 64

int main() {
    char cmd[MAX_CMD_LEN];
    char *args[MAX_ARGS];
    pid_t bg_pids[MAX_BG_PROCS];
    int bg_count = 0;

    while (1) {
        // Reap any terminated background processes (non-blocking)
        int status;
        pid_t done_pid;
        while ((done_pid = waitpid(-1, &status, WNOHANG)) > 0) {
            printf("Shell: Background process finished\n");
            // Remove from bg_pids array
            for (int i = 0; i < bg_count; i++) {
                if (bg_pids[i] == done_pid) {
                    bg_pids[i] = bg_pids[bg_count - 1];
                    bg_count--;
                    break;
                }
            }
        }

        // Print prompt
        printf("shell> ");
        fflush(stdout);

        // Read command
        if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL) {
            break; // EOF
        }

        // Remove trailing newline
        cmd[strcspn(cmd, "\n")] = 0;

        // Skip empty commands
        if (strlen(cmd) == 0) {
            continue;
        }

        // Check for background execution
        int background = 0;
        if (strlen(cmd) > 2 && cmd[strlen(cmd) - 1] == '&' && cmd[strlen(cmd) - 2] == ' ') {
            background = 1;
            cmd[strlen(cmd) - 2] = '\0'; // Remove " &"
        }

        // Tokenize command
        int argc = 0;
        char *token = strtok(cmd, " ");
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        if (argc == 0) {
            continue;
        }

        // Fork and execute
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            // Child process
            execvp(args[0], args);
            perror("execvp failed");
            exit(1);
        } else {
            // Parent process
            if (background) {
                // Background process
                if (bg_count < MAX_BG_PROCS) {
                    bg_pids[bg_count++] = pid;
                }
                printf("[Background PID: %d]\n", pid);
            } else {
                // Foreground process — wait specifically for this child
                int status;
                pid_t waited_pid;
                do {
                    waited_pid = waitpid(pid, &status, 0);
                } while (waited_pid == -1 && errno == EINTR);
                
                // While waiting, we might have reaped bg processes too (via SIGCHLD),
                // but since we use waitpid(pid, ...), we only wait for the foreground child.
                // However, bg processes may have terminated. We check again after foreground.
                while ((done_pid = waitpid(-1, &status, WNOHANG)) > 0) {
                    printf("Shell: Background process finished\n");
                    for (int i = 0; i < bg_count; i++) {
                        if (bg_pids[i] == done_pid) {
                            bg_pids[i] = bg_pids[bg_count - 1];
                            bg_count--;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Exit: reap any remaining background processes
    for (int i = 0; i < bg_count; i++) {
        int status;
        waitpid(bg_pids[i], &status, 0);
        printf("Shell: Background process finished\n");
    }

    return 0;
}