#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

typedef struct {
    int job_id;
    char operation[20];
    int input_value;
} Job;

typedef struct {
    int job_id;
    int result;
} JobResult;

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
        close(pipe1[0]); 
        close(pipe2[1]); 
        
        while (1) {
            Job job;
            printf("Enter Job ID (-1 to exit): ");
            if (scanf("%d", &job.job_id) != 1) break;
            
            if (job.job_id == -1) {
                write(pipe1[1], &job, sizeof(Job));
                break;
            }
            
            printf("Enter Operation (square, cube, double, factorial): ");
            scanf("%s", job.operation);
            
            printf("Enter Input Value: ");
            if (scanf("%d", &job.input_value) != 1) break;
            
            write(pipe1[1], &job, sizeof(Job));
            
            JobResult res;
            read(pipe2[0], &res, sizeof(JobResult));
            printf("Parent: Received Result for Job %d: %d\n\n", res.job_id, res.result);
        }
        
        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    } else { // Child
        close(pipe1[1]); 
        close(pipe2[0]); 
        
        while (1) {
            Job job;
            if (read(pipe1[0], &job, sizeof(Job)) > 0) {
                if (job.job_id == -1) {
                    break;
                }
                
                JobResult res;
                res.job_id = job.job_id;
                res.result = 0;
                
                if (strcmp(job.operation, "square") == 0) {
                    res.result = job.input_value * job.input_value;
                } else if (strcmp(job.operation, "cube") == 0) {
                    res.result = job.input_value * job.input_value * job.input_value;
                } else if (strcmp(job.operation, "double") == 0) {
                    res.result = 2 * job.input_value;
                } else if (strcmp(job.operation, "factorial") == 0) {
                    res.result = 1;
                    for (int i = 1; i <= job.input_value; i++) {
                        res.result *= i;
                    }
                } else {
                    printf("Child: Unknown operation '%s'\n", job.operation);
                }
                
                write(pipe2[1], &res, sizeof(JobResult));
            }
        }
        
        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }
    
    return 0;
}
