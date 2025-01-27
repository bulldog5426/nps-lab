#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    while (1) {
        printf("Enter the number of integers: ");
        int n;
        scanf("%d", &n);

        int arr[n];
        printf("Enter the integers: ");
        for (int i = 0; i < n; i++) {
            scanf("%d", &arr[i]);
        }

        printf("Choose an operation:\n");
        printf("1. Search for a number\n");
        printf("2. Sort in ascending order\n");
        printf("3. Sort in descending order\n");
        printf("4. Split into odd and even\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        int choice;
        scanf("%d", &choice);

        if (choice == 5) {
            send(sock, "exit", strlen("exit"), 0);
            break;
        }

        char message[BUFFER_SIZE] = {0};
        sprintf(message, "%d|", n);
        for (int i = 0; i < n; i++) {
            sprintf(message + strlen(message), "%d ", arr[i]);
        }
        sprintf(message + strlen(message), "%d ", choice);

        if (choice == 1) {
            printf("Enter the number to search: ");
            int target;
            scanf("%d", &target);
            sprintf(message + strlen(message), "%d", target);
        }

        send(sock, message, strlen(message), 0);

        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        printf("Server response: %s\n", buffer);
    }

    close(sock);
    return 0;
}
