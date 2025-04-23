#ifndef SERVER_COMMON_H
#define SERVER_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>


#define PORT 8080
#define MAX_CLIENTS 10

typedef struct {
    int socket;
    char username[50];
} ClientInfo;

typedef struct {
    char username[50];
    char password[50];
} UserCredentials;

// Global variables declarations (extern)
extern ClientInfo clients[MAX_CLIENTS];
extern int client_count;
extern pthread_mutex_t clients_mutex;

// Function declarations
void log_message(const char *format, ...);
void trim_newline(char *str);
void ensure_log_directory();


#endif // SERVER_COMMON_H