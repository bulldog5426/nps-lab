#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_SIZE 100

// Function to calculate parity bit (1 = Odd Parity, 0 = Even Parity)
char calculate_parity(char *data, int parity_type) {
    int count = 0;
    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == '1') {
            count++;
        }
    }
    return (parity_type == 1) ? ((count % 2 == 0) ? '1' : '0') : ((count % 2 == 0) ? '0' : '1');
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char data[MAX_SIZE], parity_bit, buffer[MAX_SIZE];
    int parity_type;

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

    printf("Enter binary data to send: ");
    scanf("%s", data);
    
    printf("Choose Parity Type (0 = Even, 1 = Odd): ");
    scanf("%d", &parity_type);

    // Calculate and append parity bit
    parity_bit = calculate_parity(data, parity_type);
    sprintf(buffer, "%s%c", data, parity_bit);

    // Send data
    send(sock, buffer, strlen(buffer), 0);
    printf("Sent Data with Parity Bit: %s\n", buffer);

    close(sock);
    return 0;
}
