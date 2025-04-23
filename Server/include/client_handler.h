#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "server_common.h"

// Client handling functions
void broadcast_message(char *message, int sender_sock, char *username);
void *handle_client(void *arg);

#endif // CLIENT_HANDLER_H