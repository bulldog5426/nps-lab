
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Receive available seats information
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    // Enter route choice
    fgets(buffer, BUFFER_SIZE, stdin);
    send(sock, buffer, strlen(buffer), 0);

    // Receive seat request prompt
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    // Enter number of seats to book
    fgets(buffer, BUFFER_SIZE, stdin);
    send(sock, buffer, strlen(buffer), 0);

    // Receive booking confirmation or failure message
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    close(sock);
    return 0;
}
