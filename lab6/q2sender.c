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
    int sock;
    struct sockaddr_in server_addr;
    char data[MAX_SIZE], buffer[MAX_SIZE + 10];
    unsigned int crc, poly;
    int crc_bits, choice;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("Enter data to send: ");
    scanf("%s", data);

    // Select CRC type
    printf("Select CRC type (1 = CRC-12, 2 = CRC-16, 3 = CRC-CCIP): ");
    scanf("%d", &choice);

    if (choice == 1) { poly = CRC12_POLY; crc_bits = 12; }
    else if (choice == 2) { poly = CRC16_POLY; crc_bits = 16; }
    else if (choice == 3) { poly = CRC_CCIP_POLY; crc_bits = 16; }
    else {
        printf("Invalid selection\n");
        close(sock);
        return 1;
    }

    // Compute CRC
    crc = compute_crc(data, poly, crc_bits);

    // Append CRC to data and send
    sprintf(buffer, "%s|%X", data, crc);
    send(sock, buffer, strlen(buffer), 0);
    printf("Sent Data with CRC: %s\n", buffer);

    close(sock);
    return 0;
}
