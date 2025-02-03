#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/dns_socket"
#define DATABASE_FILE "database.txt"

void resolve_domain(int client_sock) {
    char domain[256], ip[256], buffer[256];
    FILE *db_file = fopen(DATABASE_FILE, "r");

    if (!db_file) {
        perror("Database file open failed");
        snprintf(buffer, sizeof(buffer), "ERROR: Unable to access database.");
        send(client_sock, buffer, strlen(buffer), 0);
        return;
    }

    recv(client_sock, domain, sizeof(domain), 0);
    domain[strcspn(domain, "\n")] = 0; // Remove newline

    int found = 0;
    while (fscanf(db_file, "%s %s", buffer, ip) != EOF) {
        if (strcmp(buffer, domain) == 0) {
            found = 1;
            send(client_sock, ip, strlen(ip), 0);
            break;
        }
    }

    if (!found) {
        snprintf(buffer, sizeof(buffer), "ERROR: Domain not found.");
        send(client_sock, buffer, strlen(buffer), 0);
    }

    fclose(db_file);
    close(client_sock);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);  // Remove existing socket file if any

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server listening...\n");

    while ((client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len)) >= 0) {
        if (fork() == 0) {  // Child process to handle client
            resolve_domain(client_sock);
            exit(0);
        }
        close(client_sock);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
