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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ServerPort>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_port = atoi(argv[1]);

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address, client_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding server socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", server_port);

    while (1) {
        struct Packet packet;
        socklen_t client_address_len = sizeof(client_address);

        ssize_t bytes_received = recvfrom(server_socket, &packet, sizeof(packet), 0, (struct sockaddr*)&client_address, &client_address_len);

        if (bytes_received == -1) {
            perror("Error receiving packet");
            continue;
        }

        int payload_size = ntohs(packet.payload_length);

        if (bytes_received != sizeof(packet) || payload_size < 0 || payload_size > (MAX_PACKET_SIZE - 7)) {
            printf("Malformed packet received from client %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            // Respond with MALFORMED PACKET message (keeping SN and TTL unchanged)
            sendto(server_socket, &packet, sizeof(packet), 0, (struct sockaddr*)&client_address, client_address_len);
        } else {
            printf("Packet %d received from client %s:%d\n", ntohl(packet.sequence_number), inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            
            // Simulate processing delay (you can modify this part)
            usleep(5000);

            if (packet.ttl > 0) {
                // Decrement TTL and send the packet back to the client
                packet.ttl--;
                sendto(server_socket, &packet, sizeof(packet), 0, (struct sockaddr*)&client_address, client_address_len);
                printf("Packet forwarded back to client.\n");
            } else {
                printf("TTL expired, dropping packet.\n");
            }
        }
    }

    close(server_socket);

    return 0;
}

