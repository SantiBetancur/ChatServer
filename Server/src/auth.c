#include "auth.h"

void ensure_config_directory() {
    struct stat st = {0};
    if (stat("config", &st) == -1) {
        mkdir("config", 0700); // Crear directorio con permisos rwx para el propietario
    }
}

int authenticate_user(const char *username, const char *password) {
    ensure_config_directory();

    FILE *user_file = fopen("config/users.txt", "r");
    if (user_file == NULL) {
        // If file doesn't exist, only allow default user
        return (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0);
    }
    
    UserCredentials user;
    char line[100];
    while (fgets(line, sizeof(line), user_file)) {
        if (sscanf(line, "%49[^:]:%49s", user.username, user.password) == 2) {
            printf("Users: %s, Password: %s\n", user.username, user.password);
            if (strcmp(username, user.username) == 0 && 
                strcmp(password, user.password) == 0) {
                fclose(user_file);
                return 1;
            }
        }
    }
    
    fclose(user_file);
    return 0;
}