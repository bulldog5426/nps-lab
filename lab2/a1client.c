#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <TCP/UDP> <server_ip> <port>\n", argv[0]);
        return 1;
    }

    int is_udp = (strcmp(argv[1], "UDP") == 0);
    char *server_ip = argv[2];
    int port = atoi(argv[3]);

    int sock;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE], file_name[BUFFER_SIZE];

    sock = socket(AF_INET, is_udp ? SOCK_DGRAM : SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (!is_udp) {
        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            return 1;
        }
    }

    while (1) {
        printf("Enter file name (or 'stop' to exit): ");
        fgets(file_name, sizeof(file_name), stdin);
        file_name[strcspn(file_name, "\n")] = '\0';

        if (is_udp) {
            sendto(sock, file_name, strlen(file_name), 0, (struct sockaddr *)&server_addr, server_len);
            recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &server_len);
        } else {
            send(sock, file_name, strlen(file_name), 0);
            recv(sock, buffer, sizeof(buffer), 0);
        }

        if (strcmp(file_name, "stop") == 0) {
            break;
        }

        printf("Server Response:\n%s\n", buffer);
    }

    close(sock);
    return 0;
}
