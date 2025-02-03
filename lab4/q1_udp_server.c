#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void process_request(int option, struct sockaddr_in *client_addr, int sockfd) {
    char response[1024];
    pid_t pid = fork();
    
    if (pid == 0) {
        pid = getpid();
        switch (option) {
            case 1:
                snprintf(response, sizeof(response), "PID: %d\nName: John Doe\nAddress: 123, University Road", pid);
                break;
            case 2:
                snprintf(response, sizeof(response), "PID: %d\nDept: CSE\nSemester: 5\nSection: A\nCourses: DSA, Networks", pid);
                break;
            case 3:
                snprintf(response, sizeof(response), "PID: %d\nMarks in Subject: 85/100", pid);
                break;
            default:
                snprintf(response, sizeof(response), "Invalid Option");
                break;
        }

        sendto(sockfd, response, strlen(response), 0, (struct sockaddr*)client_addr, sizeof(*client_addr));
        exit(0);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("UDP Server listening on port %d...\n", PORT);

    while (1) {
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
        int option = buffer[0] - '0';
        process_request(option, &client_addr, sockfd);
    }

    return 0;
}
