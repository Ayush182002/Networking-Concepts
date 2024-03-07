#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    socklen_t clientAddrLen = sizeof(clientAddr);

    // Accept a connection from the client
    if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
        perror("Acceptance failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Receive data from client
    if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
        perror("Receiving failed");
        exit(EXIT_FAILURE);
    }

    printf("Received from client: %s\n", buffer);

    // Send data back to client
    if (send(clientSocket, buffer, strlen(buffer), 0) == -1) {
        perror("Sending failed");
        exit(EXIT_FAILURE);
    }

    printf("Sent back to client: %s\n", buffer);

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}