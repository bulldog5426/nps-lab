#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>

#define PORT 8080

void detectAndCorrectError(int data[], int r, int m) {
    int errorPos = 0;

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

        if (parity != 0) {
            errorPos += pos;
        }
    }

    if (errorPos == 0) {
        printf("No error detected.\n");
    } else {
        printf("Error detected at position %d\n", errorPos);
        data[errorPos - 1] ^= 1;
        printf("Error corrected.\n");
    }
}

void displayData(int data[], int m) {
    for (int i = 0; i < m; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    int hammingCode[7];
    int r = 3, m = 4 + r;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    listen(server_sock, 1);
    printf("Server is listening on port %d...\n", PORT);

    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    if (client_sock < 0) {
        perror("Client connection failed");
        exit(1);
    }

    recv(client_sock, hammingCode, sizeof(hammingCode), 0);
    printf("Received Hamming Code: ");
    displayData(hammingCode, m);

    detectAndCorrectError(hammingCode, r, m);

    printf("Corrected Data: ");
    displayData(hammingCode, m);

    close(client_sock);
    close(server_sock);
    return 0;
}
