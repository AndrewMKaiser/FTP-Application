#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    
    int sd; /* socket descriptor */
    int connected_sd; /* socket descriptor */
    int rc; /* return code */
    struct sockaddr_in server_address;
    struct sockaddr_in from_address;
    char buffer[100];
    char filename[100];
    int flags = 0;
    socklen_t fromLength;
    int portNumber;
    unsigned int filenameSize, filesize;
    int totalBytesReceived;
    FILE *serverFile;

    if (argc < 2){
        printf ("Usage is: server <portNumber>\n");
        exit (1);
    }

    portNumber = atoi(argv[1]); /* converting port number string to integer*/
    sd = socket (AF_INET, SOCK_STREAM, 0); /* welcoming socket (IPv4, stream socket because TCP is stream-oriented, and 0 for default protocol)*/

    server_address.sin_family = AF_INET; /* IPv4 */
    server_address.sin_port = htons (portNumber);  /* host to network short - port number (short = 2 bytes = 16 bits = port size) */
    server_address.sin_addr.s_addr = INADDR_ANY; /* defaults to local ip address*/

    rc = bind (sd, (struct sockaddr *)&server_address, sizeof(server_address)); /* binds the server address to the socket (size of server_address included for computer to understand how many bytes of data there are)*/
    if (rc < 0) {
        printf("bind error, could be another connection on port\n");
        exit(1);
    }

    listen (sd, 5); /* listens on the socket for incoming connections, allows for 5 connections to queue */
    for (;;)
    {
        connected_sd = accept (sd, (struct sockaddr *) &from_address, &fromLength); 
        if (connected_sd < 0) {
            printf("accept error");
            exit(1);
        }

        for (;;) 
        {
            totalBytesReceived = 0;

            bzero (buffer, 100);
            bzero (filename, 100);
            rc = read (connected_sd, &buffer, sizeof (int)); /* reads the size of the filename into the buffer */
            if (rc < 0) {
                perror ("couldn't read filename size");
                exit (1);
            }

            memcpy (&filenameSize, buffer, sizeof (int)); /* stores the filename size */
            filenameSize = ntohl (filenameSize);

            rc = read (connected_sd, &buffer, filenameSize); /* puts the filename in the buffer */
            if (rc < 0) {
                perror ("couldn't read filename");
                exit (1);
            }

            strcpy (filename, buffer); /* stores the filename */
            if (!(strcmp (filename, "DONE"))) { /* if client sent "DONE", sends an ACK of 4, closes the connected socket, and waits to accept another connection */
                
                totalBytesReceived = htonl (4);
                rc = write (connected_sd, &totalBytesReceived, sizeof (int));
                if (rc < 0) {
                    perror ("connection close write error");
                    exit (1);
                }

                close (connected_sd);
                break; 
            }

            rc = read (connected_sd, &buffer, sizeof (unsigned int)); /* reads the filesize to the buffer (bytes) */
            if (rc < 0) {
                perror ("couldn't read filesize");
                exit (1);
            }

            memcpy (&filesize, buffer, sizeof (unsigned int)); /* copies the filesize (in bytes) to filesize var */
            filesize = ntohl (filesize);

            serverFile = fopen (filename, "wb"); /* creates and opens a new file in write mode with the filename sent by client */

            while (filesize > 0)
            {
                rc = read (connected_sd, &buffer, 1);
                if (rc < 0) {
                    perror ("server read file error");
                    exit (1);
                }
                rc = fwrite (buffer, 1, 1, serverFile);
                if (rc < 0) {
                    perror ("server write error");
                }

                filesize--;
                totalBytesReceived++;

            }

            totalBytesReceived = htonl (totalBytesReceived);
            rc = write (connected_sd, &totalBytesReceived, sizeof (int)); /* ACK */
            
            fclose (serverFile);
        }
    }
    
    

    return 0;
}
