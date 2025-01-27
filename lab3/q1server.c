#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    pid_t pid;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the specified port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);

    // Accept a client connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    // Fork a process for full-duplex communication
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: Handle receiving messages
        printf("Child process (Receiver): PID = %d, PPID = %d\n", getpid(), getppid());
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                printf("Client disconnected.\n");
                break;
            }
            printf("Client: %s\n", buffer);
        }
    } else {
        // Parent process: Handle sending messages
        printf("Parent process (Sender): PID = %d, PPID = %d\n", getpid(), getppid());
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            printf("You: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline
            send(client_fd, buffer, strlen(buffer), 0);
            if (strcmp(buffer, "exit") == 0) {
                printf("Exiting...\n");
                break;
            }
        }
    }

    // Clean up
    close(client_fd);
    close(server_fd);
    return 0;
}
