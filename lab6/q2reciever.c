#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


#define CRC12_POLY  0x80F   // CRC-12: x^12 + x^11 + x^3 + x^2 + x + 1
#define CRC16_POLY  0x8005  // CRC-16: x^16 + x^15 + x^2 + 1
#define CRC_CCIP_POLY 0x1021 // CRC-CCIP: x^16 + x^12 + x^5 + 1

#define PORT 8080
#define MAX_SIZE 100

extern unsigned int compute_crc(char *, unsigned int, int);

int main() {
    int server_fd, client_sock, client_len;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_SIZE + 10], received_data[MAX_SIZE], *crc_str;
    unsigned int crc, received_crc, poly;
    int crc_bits, choice;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind and listen
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_len = sizeof(client_addr);
        client_sock = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);

        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        // Receive data
        recv(client_sock, buffer, MAX_SIZE + 10, 0);
        printf("Received Data: %s\n", buffer);

        // Select CRC type
        printf("Select CRC type to verify (1 = CRC-12, 2 = CRC-16, 3 = CRC-CCIP): ");
        scanf("%d", &choice);

        if (choice == 1) { poly = CRC12_POLY; crc_bits = 12; }
        else if (choice == 2) { poly = CRC16_POLY; crc_bits = 16; }
        else if (choice == 3) { poly = CRC_CCIP_POLY; crc_bits = 16; }
        else {
            printf("Invalid selection\n");
            close(client_sock);
            continue;
        }

        // Extract CRC from received data
        crc_str = strrchr(buffer, '|');
        if (crc_str) {
            *crc_str = '\0';  // Remove CRC part
            received_crc = strtol(crc_str + 1, NULL, 16);
        } else {
            printf("Error: Invalid format\n");
            close(client_sock);
            continue;
        }

        // Compute CRC on received data
        crc = compute_crc(buffer, poly, crc_bits);

        // Check CRC
        if (crc == received_crc) {
            printf("Data is CORRECT (No Corruption Detected)\n");
        } else {
            printf("Data is CORRUPT (CRC Mismatch)\n");
        }

        close(client_sock);
    }

    close(server_fd);
    return 0;
}
