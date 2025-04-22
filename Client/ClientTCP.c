#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_RECONNECT_ATTEMPTS 5
#define RECONNECT_DELAY 3 // seconds

// Hilo para enviar mensajes desde stdin al servidor
void *send_messages(void *arg) {
    int sock = *((int *)arg);
    char message[1024];

    while (fgets(message, sizeof(message), stdin)) {
        send(sock, message, strlen(message), 0);
    }
    return NULL;
}

// Hilo para recibir mensajes desde el servidor y mostrarlos
void *receive_messages(void *arg) {
    int *sock_ptr = (int *)arg;
    int sock = *sock_ptr;
    char buffer[1024];
    int bytes_read;

    while ((bytes_read = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Asegura el fin de cadena
        printf("%s", buffer); // Muestra el mensaje recibido
    }

    printf("Lost connection to server.\n");
    pthread_exit(NULL);
    return NULL;
}

// Función para intentar reconexión
int try_reconnect(int *sock, struct sockaddr_in *serv_addr, const char *username, const char *password) {
    int attempts = 0;
    char credentials[150];
    char buffer[1024] = {0};
    
    while (attempts < MAX_RECONNECT_ATTEMPTS) {
        printf("Connection lost. Attempting to reconnect (%d/%d)...\n", 
               attempts + 1, MAX_RECONNECT_ATTEMPTS);
        sleep(RECONNECT_DELAY);
        
        // Crear nuevo socket
        *sock = socket(AF_INET, SOCK_STREAM, 0);
        
        // Intentar conectar
        if (connect(*sock, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0) {
            attempts++;
            close(*sock);
            continue;
        }
        
        // Enviar credenciales
        snprintf(credentials, sizeof(credentials), "Username: %s, Password: %s", username, password);
        send(*sock, credentials, strlen(credentials), 0);
        
        // Esperar respuesta
        read(*sock, buffer, 1024);
        printf("Server response: %s\n", buffer);
        
        if (strcmp(buffer, "AUTH_SUCCESS") == 0) {
            printf("Reconnection successful!\n");
            return 1;  // Éxito
        } else {
            close(*sock);
            attempts++;
        }
    }
    
    printf("Failed to reconnect after %d attempts.\n", MAX_RECONNECT_ATTEMPTS);
    return 0;  // Fallo
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char username[50];
    char password[50];
    char credentials[150];

    // 1. Prompt user for credentials
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    snprintf(credentials, sizeof(credentials), "Username: %s, Password: %s", username, password);
    printf("ClientInfo: Credentials -> %s\n", credentials);

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configure server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr); // localhost

    // 3. Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection failed");
    return -1;
    }
    printf("ClientInfo: Connected to server\n");

    // 4. Send credentials
    send(sock, credentials, strlen(credentials), 0);
    printf("ClientInfo: Credentials sent\n");

    // 5. Wait for authentication response
    read(sock, buffer, 1024);
    printf("ClientInfo: Server response -> %s\n", buffer);

    // 6. Create threads for sending and receiving messages
    pthread_t send_thread, recv_thread;
    pthread_create(&send_thread, NULL, send_messages, &sock);
    pthread_create(&recv_thread, NULL, receive_messages, &sock);

    // 7. Wait for threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close(sock);
    return 0;
}
