#ifndef CLIENT_COMMON_H
#define CLIENT_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_RECONNECT_ATTEMPTS 5
#define RECONNECT_DELAY 3 // seconds

#endif // CLIENT_COMMON_H