 #include <stdio.h>

#define MAX_PACKETS 100  // Maximum number of packets

void leaky_bucket(int num_packets, int arrival_times[], int packet_size, int bucket_size, int leak_rate) {
    int bucket = 0;   // Current bucket size
    int time = 0;     // Time simulation
    int pkt_index = 0;// Tracks packets

    printf("\nTime\tPacket\tBucket\tStatus\n");
    printf("---------------------------------\n");

    while (pkt_index < num_packets || bucket > 0) {
        // Check if a packet arrives at this timestamp
        if (pkt_index < num_packets && arrival_times[pkt_index] == time) {
            if (bucket + packet_size <= bucket_size) {
                bucket += packet_size; // Accept packet
                printf("%d\t%d\t%d\tAccepted\n", time, packet_size, bucket);
            } else {
                printf("%d\t%d\t%d\tDropped\n", time, packet_size, bucket);
            }
            pkt_index++; // Move to next packet
        } else {
            // Process outgoing packet if the bucket is not empty
            if (bucket > 0) {
                bucket -= leak_rate;
                if (bucket < 0) bucket = 0; // Ensure bucket doesn't go negative
            }
            printf("%d\t-\t%d\tLeaking\n", time, bucket);
        }
        time++; // Increment time
    }
}

int main() {
    int num_packets, bucket_size, packet_size, leak_rate;
    int arrival_times[MAX_PACKETS];

    // Take user inputs
    printf("Enter the number of packets: ");
    scanf("%d", &num_packets);

    printf("Enter the arrival times of %d packets: ", num_packets);
    for (int i = 0; i < num_packets; i++) {
        scanf("%d", &arrival_times[i]);
    }

    printf("Enter the packet size (bytes): ");
    scanf("%d", &packet_size);

    printf("Enter the bucket size (queue capacity in bytes): ");
    scanf("%d", &bucket_size);

    printf("Enter the leak rate (bytes per second): ");
    scanf("%d", &leak_rate);

    // Run the leaky bucket algorithm
    leaky_bucket(num_packets, arrival_times, packet_size, bucket_size, leak_rate);

    return 0;
}
