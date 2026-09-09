#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT     8080
#define SERVER_IP       "127.0.0.1"
#define BUFFER_SIZE     1024

int main(void)
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    for (int snd_count = 0; snd_count < 3;snd_count++) {
        strcpy(buffer, "Hello, Server!");
        send(sockfd, buffer, strlen(buffer), 0);
        printf("Message sent to server: %s\n", buffer);

        memset(buffer, '\0', BUFFER_SIZE);

        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE,0);
        if (bytes_received < 0) {
            perror("Receive failed");
        } else {
            buffer[bytes_received] = '\0';
            printf("Message received from server: %s\n", buffer);
        }

        sleep(1);
    }

    close(sockfd);

    return 0;
}