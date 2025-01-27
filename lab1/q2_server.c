#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int is_palindrome(const char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        if (str[i] != str[len - 1 - i]) {
            return 0; // Not a palindrome
        }
    }
    return 1; // Is a palindrome
}

void count_vowels(const char *str, int *vowel_counts) {
    memset(vowel_counts, 0, 5 * sizeof(int));
    for (int i = 0; str[i]; i++) {
        switch (tolower(str[i])) {
            case 'a': vowel_counts[0]++; break;
            case 'e': vowel_counts[1]++; break;
            case 'i': vowel_counts[2]++; break;
            case 'o': vowel_counts[3]++; break;
            case 'u': vowel_counts[4]++; break;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        if (strcmp(buffer, "Halt") == 0) {
            printf("Server shutting down.\n");
            break;
        }

        int palindrome = is_palindrome(buffer);
        int vowel_counts[5];
        count_vowels(buffer, vowel_counts);
        int length = strlen(buffer);

        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE,
                 "Palindrome: %s\nLength: %d\nVowels: A=%d E=%d I=%d O=%d U=%d",
                 palindrome ? "Yes" : "No", length, vowel_counts[0], vowel_counts[1],
                 vowel_counts[2], vowel_counts[3], vowel_counts[4]);

        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}
