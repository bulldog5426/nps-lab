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
    char str1[BUFFER_SIZE], str2[BUFFER_SIZE], buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Input strings from user
    printf("Enter first string: ");
    fgets(str1, BUFFER_SIZE, stdin);
    printf("Enter second string: ");
    fgets(str2, BUFFER_SIZE, stdin);

    // Remove newline characters
    str1[strcspn(str1, "\n")] = '\0';
    str2[strcspn(str2, "\n")] = '\0';

    // Send strings to server
    snprintf(buffer, sizeof(buffer), "%s|%s", str1, str2);
    send(sock, buffer, strlen(buffer), 0);

    // Receive result from server
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Server response: %s\n", buffer);

    close(sock);
    return 0;
}
