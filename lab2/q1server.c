#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void handle_request(int sock, struct sockaddr_in *client_addr, socklen_t client_len, int is_udp) {
    char buffer[BUFFER_SIZE], file_name[BUFFER_SIZE], command[BUFFER_SIZE];
    FILE *file;
    int len;

    // Receive the file name
    if (is_udp) {
        len = recvfrom(sock, file_name, sizeof(file_name), 0, (struct sockaddr *)client_addr, &client_len);
    } else {
        len = recv(sock, file_name, sizeof(file_name), 0);
    }
    file_name[len] = '\0';

    // Open the file
    file = fopen(file_name, "r+");
    if (!file) {
        strcpy(buffer, "File not present");
        if (is_udp) {
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)client_addr, client_len);
        } else {
            send(sock, buffer, strlen(buffer), 0);
        }
        return;
    }

    while (1) {
        // Receive client command
        if (is_udp) {
            len = recvfrom(sock, command, sizeof(command), 0, (struct sockaddr *)client_addr, &client_len);
        } else {
            len = recv(sock, command, sizeof(command), 0);
        }
        command[len] = '\0';

        char option = command[0];
        if (option == '4') {
            break; // Exit
        }

        char response[BUFFER_SIZE] = "";
        if (option == '1') { // Search
            char search_str[BUFFER_SIZE];
            sscanf(command + 2, "%[^\n]", search_str);

            int count = 0;
            char line[BUFFER_SIZE];
            rewind(file);

            while (fgets(line, sizeof(line), file)) {
                char *pos = line;
                while ((pos = strstr(pos, search_str)) != NULL) {
                    count++;
                    pos += strlen(search_str);
                }
            }

            if (count > 0) {
                snprintf(response, sizeof(response), "String found %d times", count);
            } else {
                strcpy(response, "String not found");
            }

        } else if (option == '2') { // Replace
            char str1[BUFFER_SIZE], str2[BUFFER_SIZE];
            sscanf(command + 2, "%s %s", str1, str2);

            rewind(file);
            char temp_file[] = "temp.txt";
            FILE *temp = fopen(temp_file, "w");
            int replaced = 0;
            char line[BUFFER_SIZE];

            while (fgets(line, sizeof(line), file)) {
                char *pos;
                while ((pos = strstr(line, str1)) != NULL) {
                    replaced = 1;
                    char temp_line[BUFFER_SIZE];
                    strncpy(temp_line, line, pos - line);
                    temp_line[pos - line] = '\0';
                    strcat(temp_line, str2);
                    strcat(temp_line, pos + strlen(str1));
                    strcpy(line, temp_line);
                }
                fputs(line, temp);
            }
            fclose(temp);
            fclose(file);
            remove(file_name);
            rename(temp_file, file_name);

            if (replaced) {
                strcpy(response, "String replaced");
            } else {
                strcpy(response, "String not found");
            }

        } else if (option == '3') { // Reorder
            rewind(file);
            char content[BUFFER_SIZE] = "";
            char line[BUFFER_SIZE];
            while (fgets(line, sizeof(line), file)) {
                strcat(content, line);
            }
            fclose(file);

            int len = strlen(content);
            for (int i = 0; i < len - 1; i++) {
                for (int j = i + 1; j < len; j++) {
                    if (content[i] > content[j]) {
                        char temp = content[i];
                        content[i] = content[j];
                        content[j] = temp;
                    }
                }
            }

            file = fopen(file_name, "w");
            fputs(content, file);
            fclose(file);

            strcpy(response, "File reordered");
        }

        // Send response to client
        if (is_udp) {
            sendto(sock, response, strlen(response), 0, (struct sockaddr *)client_addr, client_len);
        } else {
            send(sock, response, strlen(response), 0);
        }
    }

    fclose(file);
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
