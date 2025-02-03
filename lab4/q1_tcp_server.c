#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080

void handle_client(int client_sock) {
    char buffer[1024] = {0};
    read(client_sock, buffer, sizeof(buffer));
    
    int option = buffer[0] - '0'; // Convert char to int
    char response[1024];
    
    if (fork() == 0) {
        pid_t pid = getpid();
        
        switch (option) {
            case 1: // Registration Number
                snprintf(response, sizeof(response), "PID: %d\nName: John Doe\nAddress: 123, University Road", pid);
                break;
            case 2: // Name of the Student
                snprintf(response, sizeof(response), "PID: %d\nDept: CSE\nSemester: 5\nSection: A\nCourses: DSA, Networks", pid);
                break;
            case 3: // Subject Code
                snprintf(response, sizeof(response), "PID: %d\nMarks in Subject: 85/100", pid);
                break;
            default:
                snprintf(response, sizeof(response), "Invalid Option");
                break;
        }
        
        send(client_sock, response, strlen(response), 0);
        close(client_sock);
        exit(0);
    }
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    printf("TCP Server listening on port %d...\n", PORT);

    while ((client_sock = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) >= 0) {
        handle_client(client_sock);
    }

    return 0;
}
