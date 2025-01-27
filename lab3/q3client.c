#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE], recv_buffer[BUFFER_SIZE];

    // Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // Get input from user
    printf("Enter an alphanumeric string: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline

    // Send string to server
    send(client_fd, buffer, strlen(buffer), 0);

    // Receive sorted numbers from server
    memset(recv_buffer, 0, BUFFER_SIZE);
    read(client_fd, recv_buffer, BUFFER_SIZE);
    printf("%s\n", recv_buffer);

    // Receive sorted characters from server
    memset(recv_buffer, 0, BUFFER_SIZE);
    read(client_fd, recv_buffer, BUFFER_SIZE);
    printf("%s\n", recv_buffer);

    close(client_fd);
    return 0;
}
