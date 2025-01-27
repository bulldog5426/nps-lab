#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void sort_array(int *arr, int n, int ascending) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if ((ascending && arr[j] > arr[j + 1]) || (!ascending && arr[j] < arr[j + 1])) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void split_array(int *arr, int n, int *odds, int *evens, int *odd_count, int *even_count) {
    *odd_count = 0;
    *even_count = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] % 2 == 0) {
            evens[(*even_count)++] = arr[i];
        } else {
            odds[(*odd_count)++] = arr[i];
        }
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        read(new_socket, buffer, BUFFER_SIZE);

        if (strcmp(buffer, "exit") == 0) {
            printf("Client disconnected.\n");
            break;
        }

        int n, choice;
        sscanf(buffer, "%d", &n);

        int arr[n];
        char *ptr = strchr(buffer, '|') + 1;
        for (int i = 0; i < n; i++) {
            arr[i] = atoi(strtok(i == 0 ? ptr : NULL, " "));
        }
        choice = atoi(strtok(NULL, " "));

        char response[BUFFER_SIZE] = {0};

        switch (choice) {
            case 1: { // Search
                int target = atoi(strtok(NULL, " "));
                int found = 0;
                for (int i = 0; i < n; i++) {
                    if (arr[i] == target) {
                        found = 1;
                        break;
                    }
                }
                sprintf(response, "Number %s found.", found ? "is" : "not");
                break;
            }
            case 2: // Sort ascending
                sort_array(arr, n, 1);
                for (int i = 0; i < n; i++) {
                    sprintf(response + strlen(response), "%d ", arr[i]);
                }
                break;
            case 3: // Sort descending
                sort_array(arr, n, 0);
                for (int i = 0; i < n; i++) {
                    sprintf(response + strlen(response), "%d ", arr[i]);
                }
                break;
            case 4: { // Split odd/even
                int odds[n], evens[n], odd_count, even_count;
                split_array(arr, n, odds, evens, &odd_count, &even_count);
                sprintf(response, "Odd: ");
                for (int i = 0; i < odd_count; i++) {
                    sprintf(response + strlen(response), "%d ", odds[i]);
                }
                sprintf(response + strlen(response), "\nEven: ");
                for (int i = 0; i < even_count; i++) {
                    sprintf(response + strlen(response), "%d ", evens[i]);
                }
                break;
            }
            default:
                sprintf(response, "Invalid choice.");
        }

        send(new_socket, response, strlen(response), 0);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
