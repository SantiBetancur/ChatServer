#ifndef CONNECTION_H
#define CONNECTION_H

#include "client_common.h"
void printChatStart();
// Thread functions
void *send_messages(void *arg);
void *receive_messages(void *arg);

// Connection functions
int try_reconnect(int *sock, struct sockaddr_in *serv_addr, const char *username, const char *password);

#endif // CONNECTION_H