#ifndef AUTH_H
#define AUTH_H

#include "server_common.h"

// Authentication related functions
int authenticate_user(const char *username, const char *password);
void ensure_config_directory();

#endif // AUTH_H