#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <systemd/sd-daemon.h>
#include <systemd/sd-journal.h>

#define BUFFER_SIZE     1024

int main(void)
{
    pid_t pid;
    char buffer[BUFFER_SIZE];

    pid = getpid();

    while (1) {
        int bytes_received = recv(STDIN_FILENO, buffer, BUFFER_SIZE-1,0);
        if (bytes_received <= 0) {
            sd_journal_print(LOG_ERR,"Server recv() failed\n");
            if (bytes_received == 0) {
                sd_journal_print(LOG_ERR,"Server recv() return 0, client close the socket \n");
            }
            break;
        } else {
            buffer[bytes_received] = '\0';
            sd_journal_print(LOG_ERR,"server %d  received client msg: %s \n", pid, buffer);
        }

        sprintf(buffer,"Hello, client!_%d",pid);
        int bytes_send = send(STDOUT_FILENO, buffer, strlen(buffer),0);

        if (bytes_send <= 0) {
            sd_journal_print(LOG_ERR,"Server send() failed\n");
            if (bytes_send == 0) {
                sd_journal_print(LOG_ERR,"Server send() return 0, client close the socket \n");
            }
            break;
        }
    }

    return 0;
}