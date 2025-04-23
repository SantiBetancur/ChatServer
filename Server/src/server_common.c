#include "server_common.h"

// Define global variables
ClientInfo clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_message(const char *format, ...) {
    FILE *log_file = fopen("build/logs/server_log.txt", "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        return;
    }
    
    // Get current time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
    
    // Write timestamp
    fprintf(log_file, "[%s] ", time_str);
    
    // Write formatted message
    va_list args;
    va_start(args, format);

    printf("[%s] ", time_str);

    vfprintf(log_file, format, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fclose(log_file);
}

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
