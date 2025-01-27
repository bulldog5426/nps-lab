#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

// Function to analyze the file and collect statistics
void analyze_file(FILE *file, int *alphabets, int *lines, int *spaces, int *digits, int *others, int *file_size) {
    char ch;
    *alphabets = *lines = *spaces = *digits = *others = *file_size = 0;

    while ((ch = fgetc(file)) != EOF) {
        (*file_size)++;
        if (isalpha(ch))
            (*alphabets)++;
        else if (isdigit(ch))
            (*digits)++;
        else if (isspace(ch)) {
            if (ch == '\n')
                (*lines)++;
            else
                (*spaces)++;
        } else
            (*others)++;
    }
    rewind(file); // Reset file pointer to the beginning
}

// Function to handle client requests
void handle_request(int sock, struct sockaddr_in *client_addr, socklen_t client_len, int is_udp) {
    char buffer[BUFFER_SIZE], file_name[BUFFER_SIZE];
    FILE *file;
    int len;

    while (1) {
        // Receive the file name from the client
        if (is_udp) {
            len = recvfrom(sock, file_name, sizeof(file_name), 0, (struct sockaddr *)client_addr, &client_len);
        } else {
            len = recv(sock, file_name, sizeof(file_name), 0);
        }
        file_name[len] = '\0';

        // Check for "stop" to terminate the server
        if (strcmp(file_name, "stop") == 0) {
            printf("Client requested to stop.\n");
            break;
        }

        // Open the requested file
        file = fopen(file_name, "r");
        if (!file) {
            strcpy(buffer, "File not present");
            if (is_udp) {
                sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)client_addr, client_len);
            } else {
                send(sock, buffer, strlen(buffer), 0);
            }
            continue;
        }

        // Analyze the file and collect statistics
        int alphabets, lines, spaces, digits, others, file_size;
        analyze_file(file, &alphabets, &lines, &spaces, &digits, &others, &file_size);

        // Read the file contents
        char file_contents[BUFFER_SIZE] = "";
        char line[BUFFER_SIZE];
        while (fgets(line, sizeof(line), file)) {
            strcat(file_contents, line);
        }
        fclose(file);

        // Prepare the response
        snprintf(buffer, sizeof(buffer), 
                 "File Contents:\n%s\nFile Size: %d bytes\nAlphabets: %d\nLines: %d\nSpaces: %d\nDigits: %d\nOther Characters: %d", 
                 file_contents, file_size, alphabets, lines, spaces, digits, others);

        // Send the response to the client
        if (is_udp) {
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)client_addr, client_len);
        } else {
            send(sock, buffer, strlen(buffer), 0);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <TCP/UDP> <port>\n", argv[0]);
        return 1;
    }

    int is_udp = (strcmp(argv[1], "UDP") == 0);
    int port = atoi(argv[2]);

    int sock;
    struct sockaddr_in server_addr, client_addr;

    sock = socket(AF_INET, is_udp ? SOCK_DGRAM : SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (!is_udp) {
        if (listen(sock, 5) < 0) {
            perror("Listen failed");
            return 1;
        }
        printf("Server listening on TCP port %d\n", port);
        while (1) {
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(sock, (struct sockaddr *)&client_addr, &client_len);
            if (client_sock < 0) {
                perror("Accept failed");
                continue;
            }
            handle_request(client_sock, NULL, 0, 0);
        }
    } else {
        printf("Server listening on UDP port %d\n", port);
        socklen_t client_len = sizeof(client_addr);
        handle_request(sock, &client_addr, client_len, 1);
    }

    close(sock);
    return 0;
}
