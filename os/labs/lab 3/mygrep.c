#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char * argv[])
{
    if(argc != 3) {
        printf("wrong command entered!\n");
        return 1;
    }
    char * word = argv[1]; // to search
    int fd = open(argv[2], O_RDONLY);
    if(fd == -1)
    {
        perror("error opening the file : ");
        return 1;
    }
    char buffer[1024];
    ssize_t bytes_read;
    char line[2048]; // current line holding
    int line_idx=0;
    int line_num = 1;
    while((bytes_read = read(fd, buffer, sizeof(buffer)))> 0)
    {
        for(int i=0;i<bytes_read;i++)
        {
            if(buffer[i] == '\n') {
                line[i] = '\0';
            }
            if(strstr(line,word)){
                printf("%d : %s", line_num, line);
                line_num++;
            }
        }
    }
}