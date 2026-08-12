#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Execute 'cat' with no arguments (reads from stdin)
    execlp("cat", "cat", NULL);
    
    // If execlp returns, it means execution failed
    perror("execlp failed");
    exit(1);
}