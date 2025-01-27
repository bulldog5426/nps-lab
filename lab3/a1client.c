#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void display_menu() {
    printf("\nMenu:\n");
    printf("1. Add/Subtract two integers\n");
    printf("2. Find the value of 'x' in a linear equation (ax + b = 0)\n");
    printf("3. Multiply two 2x2 matrices\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE], recv_buffer[BUFFER_SIZE];

    // Create socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    while (1) {
        display_menu();
        int choice;
        scanf("%d", &choice);

        if (choice == 4) {
            send(client_fd, "4", strlen("4"), 0);
            printf("Exiting...\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer, "%d ", choice);

        if (choice == 1) {
            int a, b;
            printf("Enter two integers: ");
            scanf("%d %d", &a, &b);
            sprintf(buffer + strlen(buffer), "%d %d", a, b);
        } else if (choice == 2) {
            int a, b;
            printf("Enter coefficients a and b (ax + b = 0): ");
            scanf("%d %d", &a, &b);
            sprintf(buffer + strlen(buffer), "%d %d", a, b);
        } else if (choice == 3) {
            int m1[2][2], m2[2][2];
            printf("Enter the first 2x2 matrix (row-wise):\n");
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    scanf("%d", &m1[i][j]);
                }
            }
            printf("Enter the second 2x2 matrix (row-wise):\n");
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    scanf("%d", &m2[i][j]);
                }
            }
            sprintf(buffer + strlen(buffer), "%d %d %d %d %d %d %d %d",
                    m1[0][0], m1[0][1], m1[1][0], m1[1][1],
                    m2[0][0], m2[0][1], m2[1][0], m2[1][1]);
        } else {
            printf("Invalid choice. Try again.\n");
            continue;
        }

        // Send data to server
        send(client_fd, buffer, strlen(buffer), 0);

        // Receive result from server
        memset(recv_buffer, 0, BUFFER_SIZE);
        read(client_fd, recv_buffer, BUFFER_SIZE);
        printf("Result: %s\n", recv_buffer);
    }

    close(client_fd);

    return 0;
}
