#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/book_socket"
#define DATABASE_FILE "books.txt"

typedef struct {
    char title[100];
    char author[100];
    char accession_no[20];
    int total_pages;
    char publisher[100];
} Book;

void insert_book(int client_sock) {
    Book book;
    recv(client_sock, &book, sizeof(Book), 0);

    FILE *db_file = fopen(DATABASE_FILE, "a");
    if (!db_file) {
        send(client_sock, "ERROR: Cannot open database.", 30, 0);
        return;
    }

    fprintf(db_file, "%s|%s|%s|%d|%s\n", book.title, book.author, book.accession_no, book.total_pages, book.publisher);
    fclose(db_file);

    send(client_sock, "Book added successfully.", 25, 0);
}

void display_books(int client_sock) {
    char buffer[512], line[512];
    FILE *db_file = fopen(DATABASE_FILE, "r");

    if (!db_file) {
        send(client_sock, "ERROR: Cannot open database.", 30, 0);
        return;
    }

    strcpy(buffer, "Books:\n");
    while (fgets(line, sizeof(line), db_file)) {
        strcat(buffer, line);
    }
    fclose(db_file);

    send(client_sock, buffer, strlen(buffer), 0);
}

void search_book(int client_sock) {
    char keyword[100], line[512], result[1024] = "Search Results:\n";
    int found = 0;

    recv(client_sock, keyword, sizeof(keyword), 0);
    FILE *db_file = fopen(DATABASE_FILE, "r");

    if (!db_file) {
        send(client_sock, "ERROR: Cannot open database.", 30, 0);
        return;
    }

    while (fgets(line, sizeof(line), db_file)) {
        if (strstr(line, keyword)) {
            strcat(result, line);
            found = 1;
        }
    }
    fclose(db_file);

    if (!found) strcpy(result, "No book found with the given title/author.");
    send(client_sock, result, strlen(result), 0);
}

void delete_book(int client_sock) {
    char accession_no[20], line[512], temp_file[] = "temp.txt";
    int found = 0;

    recv(client_sock, accession_no, sizeof(accession_no), 0);
    FILE *db_file = fopen(DATABASE_FILE, "r");
    FILE *temp_db = fopen(temp_file, "w");

    if (!db_file || !temp_db) {
        send(client_sock, "ERROR: Cannot open database.", 30, 0);
        return;
    }

    while (fgets(line, sizeof(line), db_file)) {
        if (strstr(line, accession_no)) {
            found = 1;
            continue;
        }
        fputs(line, temp_db);
    }

    fclose(db_file);
    fclose(temp_db);

    remove(DATABASE_FILE);
    rename(temp_file, DATABASE_FILE);

    send(client_sock, found ? "Book deleted successfully." : "Book not found.", 50, 0);
}

void handle_client(int client_sock) {
    int choice;
    while (recv(client_sock, &choice, sizeof(int), 0) > 0) {
        switch (choice) {
            case 1: insert_book(client_sock); break;
            case 2: delete_book(client_sock); break;
            case 3: display_books(client_sock); break;
            case 4: search_book(client_sock); break;
            case 5: send(client_sock, "Exit", 5, 0); return;
            default: send(client_sock, "Invalid option.", 15, 0);
        }
    }
    close(client_sock);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SOCKET_PATH);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);
    printf("Book Database Server Started...\n");

    while ((client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len)) >= 0) {
        if (fork() == 0) {
            handle_client(client_sock);
            exit(0);
        }
        close(client_sock);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
