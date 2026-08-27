#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
    fflush(stdout);
    setvbuf(stdout,NULL,_IONBF,0);
    printf("test stdout\n");
    int save_fd = dup(STDOUT_FILENO);
    int fd = open("foo-1.txt",(O_RDWR | O_CREAT), 0644);
    dup2(fd,STDOUT_FILENO);
    printf("test file\n");

#if 1 //with save_fd
    dup2(save_fd,STDOUT_FILENO);
#else       
    int ttyfd = open("/dev/tty",(O_RDWR), 0644);
    dup2(ttyfd,STDOUT_FILENO);
#endif
    printf("test tty\n");
}
