#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/book_socket"

typedef struct {
    char title[100];
    char author[100];
    char accession_no[20];
    int total_pages;
    char publisher[100];
} Book;

void menu() {
    printf("\n1. Insert Book\n2. Delete Book\n3. Display Books\n4. Search Book\n5. Exit\nEnter your choice: ");
}

int main() {
    int sock, choice;
    struct sockaddr_un server_addr;
    char buffer[1024], keyword[100];
    Book book;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    while (1) {
        menu();
        scanf("%d", &choice);
        send(sock, &choice, sizeof(int), 0);

        switch (choice) {
            case 1:
                printf("Enter Title: "); scanf(" %[^\n]", book.title);
                printf("Enter Author: "); scanf(" %[^\n]", book.author);
                printf("Enter Accession No: "); scanf(" %s", book.accession_no);
                printf("Enter Total Pages: "); scanf("%d", &book.total_pages);
                printf("Enter Publisher: "); scanf(" %[^\n]", book.publisher);
                send(sock, &book, sizeof(Book), 0);
                break;

            case 2:
                printf("Enter Accession No to delete: ");
                scanf("%s", keyword);
                send(sock, keyword, sizeof(keyword), 0);
                break;

            case 3:
            case 4:
                printf("Enter Title/Author (if searching): ");
                scanf(" %[^\n]", keyword);
                send(sock, keyword, sizeof(keyword), 0);
                break;

            case 5:
                close(sock);
                exit(0);
        }
        recv(sock, buffer, sizeof(buffer), 0);
        printf("%s\n", buffer);
    }
}
