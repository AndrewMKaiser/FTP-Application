# FTP Client-Server Implementation in C

This project contains a basic implementation of a File Transfer Protocol (FTP) client (`ftpc.c`) and server (`ftps.c`) written in C. The client program sends files to the server, which then writes the received files to its local file system. The server can handle up to 5 simultaneous connections.

## Project Structure

The project consists of two main files:

- `ftpc.c`: The FTP client that connects to the FTP server and sends files.
- `ftps.c`: The FTP server that accepts files from the client and writes them to its file system.

## Requirements

- A Linux or Unix-like operating system
- GCC or any standard C compiler

## Build and Run

To compile the client and server programs and create a separate directory for both the server and the client, you can use the following command:

```bash
make
```

To start the server, use the following commands:

```bash
cd ftps
./ftps <port_number>
```

To stop the server, type `CTRL + C` into the terminal to cause a keyboard interrupt.

To start the client, first open another terminal, navigate to the main directory, and use the following commands:

```bash
cd ftpc
./ftpc <ip_address> <port_number>
```

After running the client, you will be prompted to enter a filename. Press `ENTER` after typing your filename, and the file will be sent from the client to the server. Ensure that the file you wish to transfer is in the **same directory** as your client. You can transfer files up to 2<sup>32</sup> bytes, or 4 gigabytes, in length.

To exit the client and close the connection to the server, enter `DONE` when you are finished sending from that client. The server will remain open to receive files from other clients.

To remove binaries and folders after compilation, run the `make clean` command.

Alternatively, you may take the already compiled binaries in the `/bin` directory and extract them to separate directories to run. The execution of the programs remains the same.
