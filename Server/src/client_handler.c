#include "client_handler.h"

void broadcast_message(char *message, int sender_sock, char *username) {
    char formatted_message[1100];
    snprintf(formatted_message, sizeof(formatted_message), "%s: %s", username, message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].socket != sender_sock) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    int client_sock = *((int *)arg);
    char buffer[1024];
    int bytes_read;
    char client_username[50] = {0};

    // Get client username
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].socket == client_sock) {
            strncpy(client_username, clients[i].username, sizeof(client_username));
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    // While client is sending messages
    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate
        trim_newline(buffer);      // Remove spaces/newlines
    
        if (strlen(buffer) == 0) {
            printf("Received an empty message, ignoring.\n");
        } else {
            printf("Received from %s: %s\n", client_username, buffer);
            log_message("Message from %s: %s", client_username, buffer);
            // Append newline to indicate the end of the message
            strncat(buffer, "\n", sizeof(buffer) - strlen(buffer) - 1);

            // Concatenate username and message
            char message[1100];
            snprintf(message, sizeof(message), "%s: %s", client_username, buffer);
            broadcast_message(message, client_sock, client_username);
        }
    }

    // If client disconnects, remove from list
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].socket == client_sock) {
            printf("User %s disconnected\n", clients[i].username);

            // Send disconnect message to other clients
            char disconnect_message[100];
            snprintf(disconnect_message, sizeof(disconnect_message), "User %s has left the chat.\n", clients[i].username);
            for (int j = 0; j < client_count; j++) {
                if (clients[j].socket != client_sock) {
                    send(clients[j].socket, disconnect_message, strlen(disconnect_message), 0);
                }
            }
            log_message("User %s disconnected", clients[i].username);

            for (int j = i; j < client_count - 1; ++j) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(client_sock);
    free(arg);
    pthread_exit(NULL);
}