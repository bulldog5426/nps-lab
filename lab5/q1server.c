#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024

int client_count = 0;
int client_sockets[MAX_CLIENTS];

void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    char final_string[BUFFER_SIZE] = "Manipal ";
    FILE *file;

    recv(client_sock, buffer, BUFFER_SIZE, 0);
    printf("Received from Client [%s:%d]: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
    
    // Store client socket
    client_sockets[client_count++] = client_sock;

    // Append received data
    strcat(final_string, buffer);
    strcat(final_string, " ");

    if (client_count == MAX_CLIENTS) {
        strcat(final_string, "\nFinal String: ");
        strcat(final_string, "Manipal Institute of Technology");

        printf("%s\n", final_string);

        // Append to file
        file = fopen("output.txt", "w");
        if (file) {
            fprintf(file, "%s", final_string);
            fclose(file);
        }

        // Send final string to clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            send(client_sockets[i], final_string, strlen(final_string), 0);
        }
    } else if (client_count > MAX_CLIENTS) {
        // Terminate all clients if more than 2 connect
        for (int i = 0; i < client_count; i++) {
            send(client_sockets[i], "terminate session", 18, 0);
            close(client_sockets[i]);
        }
        exit(0);
    }

    close(client_sock);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, MAX_CLIENTS);

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        
        if (fork() == 0) { // Handle client in child process
            close(server_fd);
            handle_client(client_sock, client_addr);
            exit(0);
        }
        close(client_sock);
    }

    close(server_fd);
    return 0;
}
