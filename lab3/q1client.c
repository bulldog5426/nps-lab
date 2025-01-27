#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    pid_t pid;

    // Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server.\n");

    // Fork a process for full-duplex communication
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: Handle receiving messages
        printf("Child process (Receiver): PID = %d, PPID = %d\n", getpid(), getppid());
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                printf("Server disconnected.\n");
                break;
            }
            printf("Server: %s\n", buffer);
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
    return 0;
}
