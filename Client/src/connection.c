#include "connection.h"

// Thread to send messages from stdin to server
void *send_messages(void *arg) {
    int sock = *((int *)arg);
    char message[1024];

    while (fgets(message, sizeof(message), stdin)) {
        // Check if message is an exit command
        if (strcmp(message, "/exit\n") == 0 || strcmp(message, "/quit\n") == 0) {
            printf("Disconnecting from server...\n");
            close(sock);
            exit(0);  // Terminate program
        }

        send(sock, message, strlen(message), 0);
    }
    return NULL;
}

// Thread to receive messages from server and display them
void *receive_messages(void *arg) {
    int *sock_ptr = (int *)arg;
    int sock = *sock_ptr;
    char buffer[1024];
    int bytes_read;

    while ((bytes_read = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Ensure null-termination
        printf("%s", buffer); // Display received message
    }

    printf("Lost connection to server.\n");
    pthread_exit(NULL);
    return NULL;
}

// Function to attempt reconnection
int try_reconnect(int *sock, struct sockaddr_in *serv_addr, const char *username, const char *password) {
    int attempts = 0;
    char credentials[150];
    char buffer[1024] = {0};
    
    while (attempts < MAX_RECONNECT_ATTEMPTS) {
        printf("Connection lost. Attempting to reconnect (%d/%d)...\n", 
               attempts + 1, MAX_RECONNECT_ATTEMPTS);
        sleep(RECONNECT_DELAY);
        
        // Create new socket
        *sock = socket(AF_INET, SOCK_STREAM, 0);
        
        // Attempt to connect
        if (connect(*sock, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0) {
            attempts++;
            close(*sock);
            continue;
        }
        
        // Send credentials
        snprintf(credentials, sizeof(credentials), "Username: %s, Password: %s", username, password);
        send(*sock, credentials, strlen(credentials), 0);
        
        // Wait for response
        read(*sock, buffer, 1024);
        printf("Server response: %s\n", buffer);
        
        if (strcmp(buffer, "AUTH_SUCCESS") == 0) {
            printf("Reconnection successful!\n");
            return 1;  // Success
        } else {
            close(*sock);
            attempts++;
        }
    }
    
    printf("Failed to reconnect after %d attempts.\n", MAX_RECONNECT_ATTEMPTS);
    return 0;  // Failure
}