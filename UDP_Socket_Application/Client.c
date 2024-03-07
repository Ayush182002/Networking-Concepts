#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define MAX_PACKET_SIZE 1024

struct Packet {
    uint32_t sequence_number;
    uint8_t ttl;
    uint16_t payload_length;
    char payload[MAX_PACKET_SIZE - 7]; // 7 bytes for header fields
};

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <ServerIP> <ServerPort> <P> <TTL> <NumPackets>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int payload_size = atoi(argv[3]);
    int ttl = atoi(argv[4]);
    int num_packets = atoi(argv[5]);

    if (payload_size < 100 || payload_size > 1000 || ttl < 2 || ttl > 20 || ttl % 2 != 0 || num_packets < 1 || num_packets > 50) {
        fprintf(stderr, "Invalid input parameters.\n");
        exit(EXIT_FAILURE);
    }

    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_address.sin_addr);

    struct timeval start_time, end_time;
    double total_rtt = 0.0; // Total Round Trip Time

    for (int i = 0; i < num_packets; ++i) {
        struct Packet packet;
        packet.sequence_number = htonl(i);
        packet.ttl = ttl;
        packet.payload_length = htons(payload_size);

        // Fill payload with random data 
        for (int j = 0; j < payload_size; ++j) {
            packet.payload[j] = 'A' + rand() % 26;
        }

        // Record the start time
        gettimeofday(&start_time, NULL);

        sendto(client_socket, &packet, sizeof(packet), 0, (struct sockaddr*)&server_address, sizeof(server_address));
        printf("Packet %d sent to server.\n", i);

        // Receive the response packet from the server
        recvfrom(client_socket, &packet, sizeof(packet), 0, NULL, NULL);

        // Record the end time
        gettimeofday(&end_time, NULL);

        // Calculate Round Trip Time (RTT) in milliseconds
        double rtt = (end_time.tv_sec - start_time.tv_sec) * 1000.0 + (end_time.tv_usec - start_time.tv_usec) / 1000.0;
        printf("Round Trip Time for packet %d: %.2f ms\n", i, rtt);

        total_rtt += rtt;

        usleep(100000); // Sleep for 100 milliseconds (adjust as needed)
    }

    // Calculate and print average Round Trip Time
    double avg_rtt = total_rtt / num_packets;
    printf("Average Round Trip Time: %.2f ms\n", avg_rtt);

    close(client_socket);

    return 0;
}