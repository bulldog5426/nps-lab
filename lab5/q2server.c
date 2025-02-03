#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to check if two strings are anagrams
int are_anagrams(const char *str1, const char *str2) {
    int count[256] = {0};
    
    while (*str1) count[tolower(*str1++)]++;
    while (*str2) count[tolower(*str2++)]--;
    
    for (int i = 0; i < 256; i++) {
        if (count[i] != 0) return 0;
    }
    return 1;
}

// Function to log client connection details
void log_client_connection(struct sockaddr_in client_addr) {
    time_t t;
    char time_str[64];
    time(&t);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&t));
    
    printf("[%s] Connected to Client [%s:%d]\n", time_str, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE], result[BUFFER_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind and listen
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // Accept client connection
        client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        log_client_connection(client_addr);

        // Receive strings from client
        recv(client_sock, buffer, BUFFER_SIZE, 0);
        char *str1 = strtok(buffer, "|");
        char *str2 = strtok(NULL, "|");

        // Check if strings are anagrams
        if (are_anagrams(str1, str2)) {
            snprintf(result, sizeof(result), "The strings '%s' and '%s' are anagrams.", str1, str2);
        } else {
            snprintf(result, sizeof(result), "The strings '%s' and '%s' are NOT anagrams.", str1, str2);
        }

        // Send result to client
        send(client_sock, result, strlen(result), 0);
        close(client_sock);
    }

    close(server_fd);
    return 0;
}
