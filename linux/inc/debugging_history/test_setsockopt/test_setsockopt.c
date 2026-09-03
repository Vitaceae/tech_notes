#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(void) {
    int opt = 1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        perror("socket");
        return 1;
    }

#ifndef SO_REUSEPORT
    printf("SO_REUSEPORT not defined on the current system.\n");
#else
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        printf("setsockopt(SO_REUSEPORT) failed; errno=%d(%s)\n", errno, strerror(errno));
    } else {
        printf("setsockopt(SO_REUSEPORT) succeeded\n");
    }
#endif

    close(fd);
    return 0;
}