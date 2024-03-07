#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    // Send data to server
    strcpy(buffer, "Hello World!!");
    if (send(clientSocket, buffer, strlen(buffer), 0) == -1) {
        perror("Sending failed");
        exit(EXIT_FAILURE);
    }

    printf("Sent to server: %s\n", buffer);

    // Receive data from server
    if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
        perror("Receiving failed");
        exit(EXIT_FAILURE);
    }

    printf("Received from server: %s\n", buffer);

    // Close socket
    close(clientSocket);

    return 0;
}