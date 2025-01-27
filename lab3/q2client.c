#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    // Create socket
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    printf("Client is ready to send strings. Type 'exit' to quit.\n");

    while (1) {
        printf("Enter string: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline

        // Send string to server
        sendto(client_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, addr_len);

        // If the user enters "exit", terminate the client
        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting client.\n");
            break;
        }

        // Receive permutations from the server
        memset(buffer, 0, BUFFER_SIZE);
        int len = recvfrom(client_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_len);
        buffer[len] = '\0';

        if (strlen(buffer) > 0) {
            printf("Permutation: %s\n", buffer);
        }
    }

    close(client_fd);
    return 0;
}
