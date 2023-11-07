# Directories for the client and server
CLIENT_DIR = ftpc
SERVER_DIR = ftps

# Targets
all: ftpc ftps

ftpc:
	mkdir -p $(CLIENT_DIR)
	gcc -g -o $(CLIENT_DIR)/ftpc ftpc.c

ftps:
	mkdir -p $(SERVER_DIR)
	gcc -g -o $(SERVER_DIR)/ftps ftps.c

clean:
	rm -rf $(CLIENT_DIR)
	rm -rf $(SERVER_DIR)
