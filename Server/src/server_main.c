#include "server_common.h"
#include "auth.h"
#include "client_handler.h"

void ensure_log_directory() {
    struct stat st = {0};
    if (stat("build/logs", &st) == -1) {
        mkdir("build/logs", 0700); // Crea el directorio con permisos rwx para el propietario
    }
}

int main() {
    ensure_log_directory();

    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    // 1. Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Add option to reuse socket
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // 2. Configure server address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on any interface
    address.sin_port = htons(PORT);

    // 3. Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 4. Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);

    // 5. Accept multiple incoming connections
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

        // Read credentials
        read(new_socket, buffer, sizeof(buffer));
        printf("ServerInfo: credentials -> %s\n", buffer);
        
        // 6. Parse username and password
        char *username_prefix = "Username:";
        char *password_prefix = "Password:";

        char *username_start = strstr(buffer, username_prefix);
        char *password_start = strstr(buffer, password_prefix);

        char received_username[50] = {0};
        char received_password[50] = {0};

        if (username_start) {
            username_start += strlen(username_prefix);
            char *end = strchr(username_start, ','); // look for the comma
            if (end) *end = '\0'; // terminate username at the comma
            strncpy(received_username, username_start, sizeof(received_username) - 1);
        }
        
        if (password_start) {
            password_start += strlen(password_prefix);
            // Remove any trailing newline or whitespace
            char *end = strchr(password_start, '\n');
            if (end) *end = '\0';
            strncpy(received_password, password_start, sizeof(received_password) - 1);
        }

        // Remove leading spaces from username_start
        while (username_start && *username_start == ' ') {
            username_start++;
        }

        // Remove leading spaces from password_start
        while (password_start && *password_start == ' ') {
            password_start++;
        }
        printf("\033[1;33mServerInfo: Username -> %s\033[0m\n", username_start);
        printf("\033[1;33mServerInfo: Password -> %s\033[0m\n", password_start);
       
        char *response;

        // 7. Verify credentials
        if (authenticate_user(username_start, password_start)) {
            response = "AUTH_SUCCESS";
        } else {
            response = "AUTH_FAILED";
        }

        // 8. Send authentication response
        send(new_socket, response, strlen(response), 0);
        printf("Sent response: %s\n", response);

        // Connect client if auth was successful
        if (strcmp(response, "AUTH_SUCCESS") == 0) {
            pthread_mutex_lock(&clients_mutex);
            if (client_count < MAX_CLIENTS) {
                clients[client_count].socket = new_socket;
                strncpy(clients[client_count].username, received_username, sizeof(clients[client_count].username));
                client_count++;
                
                // Send connection message
                char connect_message[100];
                snprintf(connect_message, sizeof(connect_message), "User %s has joined the chat.\n", received_username);
                for (int i = 0; i < client_count - 1; i++) {  // -1 because the last one is the one who just connected
                    if (clients[i].socket != new_socket) {
                        send(clients[i].socket, connect_message, strlen(connect_message), 0);
                    }
                }
                log_message("User %s connected", received_username);
            
                // Create thread to handle client
                int *pclient = malloc(sizeof(int));
                *pclient = new_socket;
                pthread_t tid;
                pthread_create(&tid, NULL, handle_client, pclient);
            } else {
                // If too many clients, reject connection
                char *msg = "Server full\n";
                send(new_socket, msg, strlen(msg), 0);
                close(new_socket);
            }
            pthread_mutex_unlock(&clients_mutex);
        } 
    }

    return 0;
}