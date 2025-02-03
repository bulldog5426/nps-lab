#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr;
    char buffer[1024];
    socklen_t addr_len = sizeof(serv_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    int option;
    printf("Choose an option:\n1. Registration Number\n2. Name of Student\n3. Subject Code\nEnter: ");
    scanf("%d", &option);
    
    char message[10];
    sprintf(message, "%d", option);
    sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&serv_addr, &addr_len);
    printf("Server Response:\n%s\n", buffer);

    close(sockfd);
    return 0;
}
