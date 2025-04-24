#include "client_common.h"
#include "connection.h"

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char username[50];
    char password[50];
    char credentials[150];

    int auth = 0;
    while(!auth){

        // 1. Prompt user for credentials
        printf("Enter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);
        snprintf(credentials, sizeof(credentials), "Username: %s, Password: %s", username, password);
        // Print credentials in yellow
        printf("\033[1;33m%s\033[0m\n", credentials);
        

        // 2. Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);

        // 3. Configure server address
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr); // server IP
        
        // 4. Connect to the server
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection failed");
            return -1;
        }

        // 5. Send credentials
        send(sock, credentials, strlen(credentials), 0);
        // 6. Wait for authentication response
        read(sock, buffer, 1024);
        printf("\033[1;33m%s\033[0m\n", buffer);
        
        
        if (strcmp(buffer, "AUTH_SUCCESS") == 0){
            auth = 1;
            printChatStart();
        }else {
            printf("\033[1;31m%s\033[0m\n", "Verify your credentials, try again");
        } 

    }
    
    

    // 7. Create threads for sending and receiving messages
    pthread_t send_thread, recv_thread;
    pthread_create(&send_thread, NULL, send_messages, &sock);
    pthread_create(&recv_thread, NULL, receive_messages, &sock);

    // 8. Wait for threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close(sock);
    return 0;
}