#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc, char* argv[])
{
    if(argc != 3) {
        printf("command is incorrect\n");
        return 0;
    }
    int fd_src, fd_dst;
    fd_src = open(argv[1], O_RDONLY);
    if(fd_src == -1) {
        perror("error opening source : ");
        return 1;
    }
    fd_dst = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644); // standard read write permisson RWXN - 4,2,1,0
    if(fd_dst == -1)
    {
        perror("error opening destination : ");
        close(fd_src);
        return 1;
    }
    char buffer[1024];
    ssize_t bytes_read;
    while((bytes_read = read(fd_src, buffer, sizeof(buffer)))> 0){
        write(fd_dst, buffer, bytes_read); // here buffer becasue that is the ptr
    }
    close(fd_dst);
    close(fd_src);
    return 1;
}