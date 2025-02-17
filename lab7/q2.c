#include <stdio.h>

#define REPLENISH_RATE 10  // Tokens added per second (KBps)
#define BUCKET_SIZE 50     // Maximum token bucket size (KB)
#define PACKET_SIZE 15     // Each packet size (KB)
#define PACKET_INTERVAL 0.5// Packet arrival interval (seconds)

// Function to simulate token bucket mechanism
void token_bucket_simulation(double max_rate) {
    double tokens = BUCKET_SIZE; // Start with a full bucket
    double time = 0.0;           // Simulation time
    int packet_count = 0;        // Count packets sent
    double queue_time = 0.0;     // Time when queuing starts

    printf("\nTime(s)\tTokens Available\tPacket Status\n");
    printf("-------------------------------------------\n");

    while (time <= 10.0) { // Simulate for 10 seconds
        time += PACKET_INTERVAL;
        tokens += REPLENISH_RATE * PACKET_INTERVAL; // Add tokens based on rate

        if (tokens > BUCKET_SIZE) tokens = BUCKET_SIZE; // Cap at max bucket size

        // Check if packet can be sent
        if (tokens >= PACKET_SIZE) {
            tokens -= PACKET_SIZE;
            printf("%.1f\t%.1f\t\tSent\n", time, tokens);
            packet_count++;
        } else {
            if (queue_time == 0.0) queue_time = time; // First queuing event
            printf("%.1f\t%.1f\t\tQueued\n", time, tokens);
        }

        // If enforcing a max rate R
        if (max_rate > 0 && (packet_count * PACKET_SIZE) / time > max_rate) {
            printf("Max rate reached at %.1f seconds!\n", time);
            break;
        }
    }

    // (i) Tokens left at 1.5 seconds
    if (time >= 1.5) {
        printf("\n(i) Tokens left after 1.5 seconds: %.1f KB\n", tokens);
    }

    // (ii) When queuing starts
    if (queue_time > 0) {
        printf("(ii) Packets start queuing at %.1f seconds\n", queue_time);
    }

    // (iii) Max burst size if R = 20 KBps
    if (max_rate == 20) {
        double max_burst = (BUCKET_SIZE / PACKET_SIZE) * PACKET_SIZE;
        printf("(iii) Maximum burst size at R = 20 KBps: %.1f KB\n", max_burst);
    }
}

int main() {
    printf("\n--- Case 1: Infinite Maximum Rate (R = ∞) ---\n");
    token_bucket_simulation(0); // Infinite max rate

    printf("\n--- Case 2: Limited Maximum Rate (R = 20 KBps) ---\n");
    token_bucket_simulation(20); // Limited rate case

    return 0;
}
