#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define SERVER_IP "10.2.81.9"
#define BUFFER_SIZE 1024

void *receive_messages(void *arg);
void *send_messages(void *arg);

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

    // Create two threads for handling the client: one for sending and one for receiving
    pthread_t receive_thread, send_thread;

    if (pthread_create(&receive_thread, NULL, receive_messages, (void*)&clientSocket) != 0 ||
        pthread_create(&send_thread, NULL, send_messages, (void*)&clientSocket) != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    // Wait for both threads to finish before exiting
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    // Close the client socket
    close(clientSocket);

    return 0;
}

void *receive_messages(void *arg) {
    int clientSocket = *((int*)arg);
    char buffer[BUFFER_SIZE];

    while (1) {
        // Receive response from server
        if (recv(clientSocket, buffer, BUFFER_SIZE, 0) == -1) {
            perror("Receiving failed");
            break;
        }

        printf("Received from server: %s\n", buffer);

        // Check for "Bye" message to close the chat session
        if (strcmp(buffer, "Bye") == 0) {
            printf("Chat session closed by server.\n");
            break;
        }

        // Print "Enter your message"
        printf("Enter your message: ");
    }

    pthread_exit(NULL);
}

void *send_messages(void *arg) {
    int clientSocket = *((int*)arg);
    char buffer[BUFFER_SIZE];

    while (1) {
        // Print "Enter your message"
        printf("Enter your message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove the newline character from the input

        // Send data to server
        if (send(clientSocket, buffer, strlen(buffer), 0) == -1) {
            perror("Sending failed");
            break;
        }

        // Check for "Bye" message to close the chat session
        if (strcmp(buffer, "Bye") == 0) {
            printf("Chat session closed by client.\n");
            break;
        }
    }

    pthread_exit(NULL);
}