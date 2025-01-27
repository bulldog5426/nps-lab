#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to swap two characters
void swap(char* x, char* y) {
    char temp = *x;
    *x = *y;
    *y = temp;
}

// Function to generate and print all permutations of a string
void permute(char* str, int l, int r, struct sockaddr_in* client_addr, int sock, socklen_t addr_len) {
    if (l == r) {
        sendto(sock, str, strlen(str), 0, (struct sockaddr*)client_addr, addr_len);
    } else {
        for (int i = l; i <= r; i++) {
            swap((str + l), (str + i));
            permute(str, l + 1, r, client_addr, sock, addr_len);
            swap((str + l), (str + i)); // backtrack
        }
    }
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Create socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the specified port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Receive string from client
        memset(buffer, 0, BUFFER_SIZE);
        int len = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
        buffer[len] = '\0';

        if (strcmp(buffer, "exit") == 0) {
            printf("Server is shutting down.\n");
            break;
        }

        printf("Received string from client: %s\n", buffer);

        // Generate and send all permutations to the client
        permute(buffer, 0, strlen(buffer) - 1, &client_addr, server_fd, addr_len);
    }

    close(server_fd);
    return 0;
}
