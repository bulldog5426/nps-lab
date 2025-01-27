#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void add_subtract(int a, int b, char *result) {
    int sum = a + b;
    int diff = a - b;
    sprintf(result, "Addition: %d, Subtraction: %d", sum, diff);
}

void solve_linear_equation(int a, int b, char *result) {
    if (a == 0) {
        if (b == 0)
            strcpy(result, "Infinite solutions (0x = 0).");
        else
            strcpy(result, "No solution (0x = non-zero).");
    } else {
        float x = (float)-b / a;
        sprintf(result, "x = %.2f", x);
    }
}

void multiply_matrices(int m1[2][2], int m2[2][2], char *result) {
    int res[2][2];
    memset(res, 0, sizeof(res));

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                res[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }

    sprintf(result, "Matrix Multiplication Result:\n[%d %d]\n[%d %d]",
            res[0][0], res[0][1], res[1][0], res[1][1]);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE], result[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for client connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for connections on port %d...\n", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        memset(result, 0, BUFFER_SIZE);

        // Receive data from client
        read(new_socket, buffer, BUFFER_SIZE);

        if (strncmp(buffer, "4", 1) == 0) {
            printf("Client exited.\n");
            break;
        }

        int choice = buffer[0] - '0';

        if (choice == 1) {
            int a, b;
            sscanf(buffer + 2, "%d %d", &a, &b);
            add_subtract(a, b, result);
        } else if (choice == 2) {
            int a, b;
            sscanf(buffer + 2, "%d %d", &a, &b);
            solve_linear_equation(a, b, result);
        } else if (choice == 3) {
            int m1[2][2], m2[2][2];
            sscanf(buffer + 2, "%d %d %d %d %d %d %d %d",
                   &m1[0][0], &m1[0][1], &m1[1][0], &m1[1][1],
                   &m2[0][0], &m2[0][1], &m2[1][0], &m2[1][1]);
            multiply_matrices(m1, m2, result);
        } else {
            strcpy(result, "Invalid option");
        }

        // Send result to client
        send(new_socket, result, strlen(result), 0);
    }

    close(new_socket);
    close(server_fd);

    return 0;
}
