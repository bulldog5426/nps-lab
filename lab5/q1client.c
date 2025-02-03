#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if (argc != 2) {
        printf("Usage: %s <message>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    send(sock, argv[1], strlen(argv[1]), 0);
    recv(sock, buffer, BUFFER_SIZE, 0);

    printf("Server response: %s\n", buffer);

    close(sock);
    return 0;
}
