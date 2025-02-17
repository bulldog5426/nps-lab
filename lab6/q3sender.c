#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>

#define PORT 8080

void calculateParity(int data[], int r, int m) {
    for (int i = 0; i < r; i++) {
        int pos = pow(2, i);
        int parity = 0;
        
        for (int j = pos - 1; j < m; j += (pos * 2)) {
            for (int k = 0; k < pos; k++) {
                if (j + k < m) {
                    parity ^= data[j + k];
                }
            }
        }
        data[pos - 1] = parity;
    }
}

void displayData(int data[], int m) {
    for (int i = 0; i < m; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

int main() {
    int d[4], hammingCode[7] = {0};
    int r = 3, m = 4 + r;
    int sock;
    struct sockaddr_in server_addr;

    printf("Enter 4-bit data (one by one):\n");
    for (int i = 0; i < 4; i++) {
        scanf("%d", &d[i]);
    }

    int j = 0;
    for (int i = 0; i < m; i++) {
        if ((i + 1) & (i)) {
            hammingCode[i] = d[j++];
        }
    }

    calculateParity(hammingCode, r, m);

    printf("Generated Hamming Code: ");
    displayData(hammingCode, m);

    // Introduce error (optional)
    int errorBit;
    printf("Enter error bit position (1-7) or 0 for no error: ");
    scanf("%d", &errorBit);
    
    if (errorBit > 0 && errorBit <= m) {
        hammingCode[errorBit - 1] ^= 1;
        printf("Transmitted data with error: ");
        displayData(hammingCode, m);
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    send(sock, hammingCode, sizeof(hammingCode), 0);
    printf("Data sent to receiver.\n");

    close(sock);
    return 0;
}
