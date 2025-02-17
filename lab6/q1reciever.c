#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_SIZE 100

// Function to check if parity is correct
int check_parity(char *data, int parity_type) {
    int count = 0;
    int len = strlen(data);
    char received_parity = data[len - 1];  // Last bit is the parity bit
    data[len - 1] = '\0'; // Remove parity bit from data

    // Count number of 1s
    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == '1') {
            count++;
        }
    }

    // Compute expected parity bit
    char expected_parity = (parity_type == 1) ? ((count % 2 == 0) ? '1' : '0') : ((count % 2 == 0) ? '0' : '1');

    return (received_parity == expected_parity);
}

int main() {
    int server_fd, client_sock, client_len;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_SIZE];

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind and listen
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_len = sizeof(client_addr);
        client_sock = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);

        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        // Receive data
        recv(client_sock, buffer, MAX_SIZE, 0);
        printf("Received Data: %s\n", buffer);

        // Prompt for parity type
        int parity_type;
        printf("Choose Parity Type to verify (0 = Even, 1 = Odd): ");
        scanf("%d", &parity_type);

        // Check if data is corrupt
        if (check_parity(buffer, parity_type)) {
            printf("Data is CORRECT (No Corruption Detected)\n");
        } else {
            printf("Data is CORRUPT (Parity Mismatch)\n");
        }

        close(client_sock);
    }

    close(server_fd);
    return 0;
}
