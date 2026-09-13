#include <stdio.h>
#include <fcntl.h> // cntl as control.
#include <unistd.h>
int main(int argc, char* argv[])
{
    int fd;
    if(argc != 2) { // argv[0] is reserved for the program own path
        printf("wrong command entered\n");
        return 1;
    }
    // open the file first to read.
    fd = open(argv[1], O_RDONLY); //name, flag, mode
    // opened
    // now have to read and print.
    // for storing buffer is req.
    char buffer[1024];
    int line_count=0;
    ssize_t bytes_read ; 
    while((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        for(int i=0;i<1024;i++) // why not to do 1024, and use byte size, reason content in the file could be less that that which result in the printing
        // the garbage.
        {
            write(1, &buffer[i], 1); // read 1 byte and print to the stdout
            if(buffer[i] == '\n')line_count++;
            if(line_count == 5) {
                close(fd);
                return 1;
            }
        }
    }

    return 0;
}