#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to sort numbers in ascending order
void sort_numbers(char* str, char* sorted_nums) {
    char numbers[BUFFER_SIZE];
    int num_count = 0;

    // Extract numbers
    for (int i = 0; str[i] != '\0'; i++) {
        if (isdigit(str[i])) {
            numbers[num_count++] = str[i];
        }
    }
    numbers[num_count] = '\0';

    // Sort numbers
    for (int i = 0; i < num_count - 1; i++) {
        for (int j = i + 1; j < num_count; j++) {
            if (numbers[i] > numbers[j]) {
                char temp = numbers[i];
                numbers[i] = numbers[j];
                numbers[j] = temp;
            }
        }
    }

    strcpy(sorted_nums, numbers);
}

// Function to sort characters in descending order
void sort_characters(char* str, char* sorted_chars) {
    char chars[BUFFER_SIZE];
    int char_count = 0;

    // Extract characters
    for (int i = 0; str[i] != '\0'; i++) {
        if (isalpha(str[i])) {
            chars[char_count++] = str[i];
        }
    }
    chars[char_count] = '\0';

    // Sort characters
    for (int i = 0; i < char_count - 1; i++) {
        for (int j = i + 1; j < char_count; j++) {
            if (chars[i] < chars[j]) {
                char temp = chars[i];
                chars[i] = chars[j];
                chars[j] = temp;
            }
        }
    }

    strcpy(sorted_chars, chars);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE], sorted_nums[BUFFER_SIZE], sorted_chars[BUFFER_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept client connection
    new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (new_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected\n");

    // Receive data from client
    read(new_socket, buffer, BUFFER_SIZE);
    printf("Received string from client: %s\n", buffer);

    pid_t pid = fork(); // Create child process

    if (pid == 0) {  // Child process
        sort_numbers(buffer, sorted_nums);
        snprintf(buffer, BUFFER_SIZE, "Child (PID %d): Sorted numbers: %s", getpid(), sorted_nums);
        send(new_socket, buffer, strlen(buffer), 0);
    } else {  // Parent process
        sort_characters(buffer, sorted_chars);
        snprintf(buffer, BUFFER_SIZE, "Parent (PID %d): Sorted characters: %s", getpid(), sorted_chars);
        send(new_socket, buffer, strlen(buffer), 0);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
