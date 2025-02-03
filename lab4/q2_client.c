#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/dns_socket"

int main() {
    int sock;
    struct sockaddr_un server_addr;
    char domain[256], buffer[256];

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter domain name to resolve: ");
    scanf("%s", domain);

    send(sock, domain, strlen(domain), 0);
    recv(sock, buffer, sizeof(buffer), 0);
    
    printf("Resolved IP: %s\n", buffer);

    close(sock);
    return 0;
}
