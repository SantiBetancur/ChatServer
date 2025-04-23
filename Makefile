CC = gcc
CFLAGS = -Wall -IClient/include -IServer/include
BUILD_DIR = build

# Server files
SERVER_SRC = Server/src/server_main.c Server/src/server_common.c Server/src/auth.c Server/src/client_handler.c
SERVER_OBJ = $(SERVER_SRC:%.c=$(BUILD_DIR)/%.o)
SERVER_EXE = $(BUILD_DIR)/ServerApp

# Client files
CLIENT_SRC = Client/src/client_main.c Client/src/connection.c
CLIENT_OBJ = $(CLIENT_SRC:%.c=$(BUILD_DIR)/%.o)
CLIENT_EXE = $(BUILD_DIR)/ClientApp

all: $(SERVER_EXE) $(CLIENT_EXE)

$(SERVER_EXE): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_EXE): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)  # Crea la carpeta si no existe
	$(CC) $(CFLAGS) -c $< -o $@

run_server: $(SERVER_EXE)
	@echo "Ejecutando el servidor..."
	@$(SERVER_EXE)

run_client: $(CLIENT_EXE)
	@echo "Ejecutando el cliente..."
	@$(CLIENT_EXE)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean