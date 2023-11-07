# Directories for the client and server
CLIENT_DIR = client
SERVER_DIR = server

# Targets
all: ftpc ftps

ftpc:
	mkdir -p $(CLIENT_DIR)
	gcc -g -o $(CLIENT_DIR)/client ftpc.c

ftps:
	mkdir -p $(SERVER_DIR)
	gcc -g -o $(SERVER_DIR)/server ftps.c

clean:
	rm -rf $(CLIENT_DIR)
	rm -rf $(SERVER_DIR)
