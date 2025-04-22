#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10

typedef struct {
    int socket;
    char username[50];
} ClientInfo;

// Arreglo para almacenar los sockets de los clientes conectados
ClientInfo clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void trim_newline(char *str) {
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
    // Optional: remove leading whitespace too
    // Shift string forward if needed
    size_t start = 0;
    while (isspace((unsigned char)str[start])) start++;
    if (start > 0) memmove(str, str + start, len - start + 1);
}

// Función para enviar un mensaje a todos los clientes excepto al que lo envió
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

// Función manejadora para cada cliente (se ejecuta en un hilo)
void *handle_client(void *arg) {
    int client_sock = *((int *)arg);
    char msg[50];
    char buffer[1024];
    int bytes_read;
    char client_username[50] = {0};

    //obtener el nombre de usuario del cliente
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].socket == client_sock) {
            strncpy(client_username, clients[i].username, sizeof(client_username));
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    // Mientras el cliente esté enviando mensajes
    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate
        trim_newline(buffer);
            // Remove spaces/newlines
    
        if (strlen(buffer) == 0) {
            printf("Received an empty message, ignoring.\n");
        } else {
            printf("Received from %s: %s\n", client_username, buffer);
            // Append newline to indicate the end of the message
            strncat(buffer, "\n", sizeof(buffer) - strlen(buffer) - 1);
            broadcast_message(buffer, client_sock, client_username);
        }
    }

    // Si el cliente se desconecta, lo eliminamos de la lista
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i].socket == client_sock) {
            printf("User %s disconnected\n", clients[i].username);
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

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Hardcoded valid credentials
    const char *valid_username = "admin";
    const char *valid_password = "1234";


    // 1. Crear socket del servidor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configurar dirección y puerto del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Escucha en cualquier interfaz
    address.sin_port = htons(PORT);

    // 3. Enlazar el socket al puerto
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // 4. Escuchar conexiones entrantes
    listen(server_fd, 5);
    printf("Server listening on port %d\n", PORT);

    // 5. Aceptar múltiples conexiones entrantes
    while (1) {

        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

         //  Read credentials
        read(new_socket, buffer, sizeof(buffer));
        printf("ServerInfo: credentials -> %s\n", buffer);
        

        // 6. Parse username and password
        // Parse username and password
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
        printf("ServerInfo: Username -> %s\n", username_start);
        printf("ServerInfo: Password -> %s\n", password_start);
        char *response;

        // 7. Verify credentials

        if (    
            strcmp(username_start, valid_username) == 0 &&
            strcmp(password_start, valid_password) == 0) {
            response = "AUTH_SUCCESS";
        } else {
            response = "AUTH_FAILED";
        }

        // 8. Send authentication response
        send(new_socket, response, strlen(response), 0);
        printf("Sent response: %s\n", response);

        //Connect client if auth was success
        if (response == "AUTH_SUCCESS"){
            pthread_mutex_lock(&clients_mutex);
            if (client_count < MAX_CLIENTS) {
                clients[client_count].socket = new_socket;
                strncpy(clients[client_count].username, received_username, sizeof(clients[client_count].username));
                client_count++;
    
                // Crear hilo para atender al cliente
                int *pclient = malloc(sizeof(int));
                *pclient = new_socket;
                pthread_t tid;
                pthread_create(&tid, NULL, handle_client, pclient);
            } else {
                // Si hay demasiados clientes, rechazar conexión
                char *msg = "Server full\n";
                send(new_socket, msg, strlen(msg), 0);
                close(new_socket);
            }
            pthread_mutex_unlock(&clients_mutex);
        }else {
            char *msg = "Authentication falied, try again\n";
            send(new_socket, msg, strlen(msg), 0);
        }
       
    }

    return 0;
}