#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void *handle_client_receive(void *arg);
void *handle_client_send(void *arg);

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

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

    while (1) {
        // Accept a connection from the client
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
            perror("Acceptance failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Create two threads for handling the client: one for sending and one for receiving
        pthread_t receive_thread, send_thread;

        if (pthread_create(&receive_thread, NULL, handle_client_receive, (void*)&clientSocket) != 0 ||
            pthread_create(&send_thread, NULL, handle_client_send, (void*)&clientSocket) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        // Wait for both threads to finish before accepting a new connection
        pthread_join(receive_thread, NULL);
        pthread_join(send_thread, NULL);
    }

    // Close server socket (this part is unreachable in this example)
    close(serverSocket);

    return 0;
}

void *handle_client_receive(void *arg) {
    int clientSocket = *((int*)arg);
    char buffer[BUFFER_SIZE];

    while (1) {
        // Receive data from client
        if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
            perror("Receiving failed");
            break;
        }

        printf("Received from client: %s\n", buffer);

        // Check for "Bye" message to close the chat session
        if (strcmp(buffer, "Bye") == 0) {
            printf("Chat session closed by client.\n");
            break;
        }
    }

    pthread_exit(NULL);
}

void *handle_client_send(void *arg) {
    int clientSocket = *((int*)arg);
    char buffer[BUFFER_SIZE];

    while (1) {
        // Print "Enter server's response"
        printf("Enter server's response: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove the newline character from the input

        // Send response back to client
        if (send(clientSocket, buffer, strlen(buffer), 0) == -1) {
            perror("Sending failed");
            break;
        }

        // Check for "Bye" message to close the chat session
        if (strcmp(buffer, "Bye") == 0) {
            printf("Chat session closed by server.\n");
            break;
        }
    }

    pthread_exit(NULL);
}