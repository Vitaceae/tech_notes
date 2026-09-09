#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <systemd/sd-daemon.h>
#include <netinet/in.h>
#include <systemd/sd-journal.h>

#define SERVER_PORT     8080
#define BUFFER_SIZE     1024

int main(void)
{
    int handle = -1;
    int n = sd_listen_fds(0);

    if (n <= 0) {
        sd_journal_print(LOG_INFO, "No fd received\n");
        handle = socket(AF_INET, SOCK_STREAM, 0);

        if (handle == -1) {
            perror("socket");
            exit(1);
        }

        struct sockaddr_in sock_server;
        sock_server.sin_family = AF_INET;
        sock_server.sin_addr.s_addr = INADDR_ANY;
        sock_server.sin_port = htons(SERVER_PORT);

        if (bind(handle, (struct sockaddr *)&sock_server, sizeof(sock_server)) == -1) {
            sd_journal_print(LOG_INFO, "server bind error\n");
            exit(1);
        }

        if (listen(handle, 10) == -1) {
            sd_journal_print(LOG_INFO, "server listen error\n");
            exit(1);
        }
    } else {
        sd_journal_print(LOG_INFO, "Received %d fd(s) from systemd\n", n);

        int last_handle = SD_LISTEN_FDS_START + n;
        bool fd_check_ok = false;

        for (handle = SD_LISTEN_FDS_START; handle < last_handle; ++handle) {
            if (sd_is_socket_inet (handle, AF_INET, SOCK_STREAM, 1, (uint16_t) SERVER_PORT) > 0) {
                sd_journal_print(LOG_INFO, "IPv4 TCP socket listening on port %d\n", SERVER_PORT);
                fd_check_ok = true;
                break;
            }
        }

        if (fd_check_ok != true) {
            sd_journal_print(LOG_ERR, "No fd found for port %d\n", SERVER_PORT);
            return 1;
        }
    }

    struct sockaddr_storage sock_client;
    socklen_t client_addr_len = sizeof(sock_client);
    char buffer[BUFFER_SIZE];
    int client_fd;

    while (1) {
        client_fd = accept(handle, (struct sockaddr *)&sock_client, &client_addr_len);
        if (client_fd < 0) {
            sd_journal_print(LOG_INFO,"Accept failed\n");
            continue;
        }

        while (1) {
            int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                sd_journal_print(LOG_INFO,"no data received");
                break;
            } else {
                buffer[bytes_received] = '\0';
                sd_journal_print(LOG_INFO,"received from client: %s\n", buffer);
            }

            strcpy(buffer, "Hello, client!");
            int bytes_send = send(client_fd, buffer, strlen(buffer), 0);
            if (bytes_send <= 0) {
                sd_journal_print(LOG_INFO,"Server Send failed");
                break;
            }
        }
        close(client_fd);
    }

    return 0;
}