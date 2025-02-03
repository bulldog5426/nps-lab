#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10

typedef struct {
    char route[50];
    int total_seats;
    int booked_seats;
} RouteInfo;

// Define two routes
RouteInfo routes[2] = {
    {"City A to City B", 20, 0},
    {"City C to City D", 15, 0}
};

// Mutex for thread safety
pthread_mutex_t lock;

// Function to handle client requests
void *handle_client(void *arg) {
    int client_sock = *((int *)arg);
    free(arg);

    char buffer[1024];
    int route_choice, seats_requested;
    
    // Send available seats info to client
    snprintf(buffer, sizeof(buffer), "Available Seats:\n1. %s - %d seats available\n2. %s - %d seats available\nEnter your choice (1/2): ",
             routes[0].route, routes[0].total_seats - routes[0].booked_seats,
             routes[1].route, routes[1].total_seats - routes[1].booked_seats);
    send(client_sock, buffer, strlen(buffer), 0);
    
    // Receive client route choice
    recv(client_sock, buffer, sizeof(buffer), 0);
    route_choice = atoi(buffer) - 1;

    if (route_choice < 0 || route_choice >= 2) {
        strcpy(buffer, "Invalid choice. Disconnecting...\n");
        send(client_sock, buffer, strlen(buffer), 0);
        close(client_sock);
        return NULL;
    }

    // Ask for the number of seats
    strcpy(buffer, "Enter the number of seats you want to book: ");
    send(client_sock, buffer, strlen(buffer), 0);
    recv(client_sock, buffer, sizeof(buffer), 0);
    seats_requested = atoi(buffer);

    pthread_mutex_lock(&lock);
    
    // Check seat availability
    if (seats_requested <= 0 || seats_requested > (routes[route_choice].total_seats - routes[route_choice].booked_seats)) {
        snprintf(buffer, sizeof(buffer), "Not enough seats available. Only %d seats left.\n", routes[route_choice].total_seats - routes[route_choice].booked_seats);
    } else {
        // Book the seats
        routes[route_choice].booked_seats += seats_requested;
        snprintf(buffer, sizeof(buffer), "Booking successful! %d seats booked on %s.\n", seats_requested, routes[route_choice].route);
    }
    
    pthread_mutex_unlock(&lock);

    // Send response to client and close connection
    send(client_sock, buffer, strlen(buffer), 0);
    close(client_sock);
    return NULL;
}

int main() {
    int server_fd, *client_sock, client_len;
    struct sockaddr_in server_addr, client_addr;
    pthread_t thread_id;

    // Initialize mutex
    pthread_mutex_init(&lock, NULL);

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind and listen
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, MAX_CLIENTS);

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_len = sizeof(client_addr);
        client_sock = malloc(sizeof(int));
        *client_sock = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_len);

        if (*client_sock < 0) {
            perror("Accept failed");
            free(client_sock);
            continue;
        }

        // Create a new thread for each client
        pthread_create(&thread_id, NULL, handle_client, client_sock);
        pthread_detach(thread_id);  // Automatically cleans up thread resources when done
    }

    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}
