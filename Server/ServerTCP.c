#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Hardcoded valid credentials
    const char *valid_username = "admin";
    const char *valid_password = "1234";

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 4. Accept connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // 5. Read credentials
    read(new_socket, buffer, sizeof(buffer));
    printf("Received credentials: %s\n", buffer);

    // 6. Parse username and password
    char *received_username = strtok(buffer, ":");
    char *received_password = strtok(NULL, ":");

    char *response;

    // 7. Verify credentials
    if (received_username && received_password &&
        strcmp(received_username, valid_username) == 0 &&
        strcmp(received_password, valid_password) == 0) {
        response = "AUTH_SUCCESS";
    } else {
        response = "AUTH_FAILED";
    }

    // 8. Send authentication response
    send(new_socket, response, strlen(response), 0);
    printf("Sent response: %s\n", response);

    close(new_socket);
    close(server_fd);
    return 0;
}
