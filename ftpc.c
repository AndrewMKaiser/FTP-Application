#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

unsigned int getFileLength (FILE *fp);
int sendfile(int sd, FILE *clientFile);

int main (int argc, char *argv[])
{
    int sd;
    struct sockaddr_in server_address;
    char buffer[100];
    int portNumber, filenameSize, clientFilesize, serverBytesReceived;
    char serverIP[29];
    int rc = 0;
    FILE *clientFile;

    if (argc < 3){
        printf ("usage is client <ipaddr> <port>\n");
        exit (1);
    }

    

    sd = socket (AF_INET, SOCK_STREAM, 0);
    portNumber = strtol (argv[2], NULL, 10); /* converts port number string to long */

    strcpy (serverIP, argv[1]); /* stores server IP in serverIP*/

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons (portNumber);
    server_address.sin_addr.s_addr = inet_addr (serverIP);

    if (connect (sd, (struct sockaddr *)&server_address, sizeof (struct sockaddr_in)) < 0) { /* connects client to server socket */
        close (sd);
        perror ("error connecting stream socket");
        exit (1);
    }

    for (;;)
    {
        bzero (buffer, 100); /* zeroes-out buffer*/
        printf ("Please enter the filename of the file to send to the server: \n");
        scanf ("%s", buffer);

        if (!(strcmp (buffer, "DONE"))) { /* initiates exit process if user entered "DONE" */

            filenameSize = htonl (4);
            rc = write (sd, &filenameSize, sizeof (int)); /* sends length of "DONE" (4) to server */
            if (rc < 0) {
                perror ("'DONE'");
            }

            rc = write (sd, buffer, strlen (buffer)); /* sends "DONE" to server */
            if (rc < 0) {
                perror ("'DONE' filename write error");
                exit (1);
            }

            rc = read (sd, buffer, 4); /* reads in '4' ACK from server */
            if (rc < 0) {
                perror ("connection close read error");
                exit (1);
            }

            memcpy (&serverBytesReceived, buffer, sizeof (int));
            serverBytesReceived = ntohl (serverBytesReceived);

            if (serverBytesReceived != 4) {
                perror ("connection close error (0 ACK not received)");
                exit (1);
            }

            close (sd);
            break;
        }

        clientFile = fopen (buffer, "rb");
        if (clientFile == NULL) { /* opens file w/ filename and punts if filename is invalid*/
            printf ("filename '%s' not found", buffer);
            exit (1);
        } 

        filenameSize = htonl (strlen (buffer));

        rc = write (sd, &filenameSize, sizeof (int)); /* writes the size of the filename to the socket */
        if (rc < 0) {
            perror ("send filename size");
            exit (1);
        }

        rc = write (sd, buffer, strlen (buffer)); /* writes the filename to the socket */
        if (rc < 0) {
            perror ("send filename");
            exit (1);
        }

        clientFilesize = htonl (getFileLength (clientFile));

        rc = write (sd, &clientFilesize, sizeof (unsigned int)); /* writes filesize to the socket*/
        if (rc < 0) {
            perror ("send filesize");
            exit (1);
        }
        
        rc = sendfile (sd, clientFile); /* sends contents of file to socket */
        if (rc != 0) {
            perror ("sendfile");
            exit (1);
        }

        bzero (buffer, 100); /* clears buffer for number of bytes received by server */
        rc = read (sd, buffer, sizeof (int));

        memcpy (&serverBytesReceived, buffer, sizeof (int));
        serverBytesReceived = ntohl (serverBytesReceived);

        printf ("Server has received %d bytes\n", serverBytesReceived);
        fclose (clientFile);

    }

    return (0);

}


/************************************************************************/
/* This function will return the filesize, in bytes, of the client file */
/************************************************************************/
#define FILE_TRANSFER_BUFFER_SIZE 50

unsigned int getFileLength(FILE* fp)
{
    fseek(fp, 0, SEEK_END);
    unsigned int len = (unsigned int) ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return len;
}

int sendfile(int sd, FILE *clientFile)
{
    unsigned char buffer [FILE_TRANSFER_BUFFER_SIZE];
    int rc;
    int numberOfBytes;

    bzero (buffer, FILE_TRANSFER_BUFFER_SIZE);

    numberOfBytes = fread (buffer, 1, 1, clientFile);
    if (numberOfBytes < 0) {
        perror ("invalid clientFile in readfile()");
        exit (1);
    }
    
    while (numberOfBytes > 0) {
        rc = write (sd, buffer, 1); /* should always write the same number of elements that fread() reads */
        if (numberOfBytes != rc) {
            perror ("writing to sd");
            exit (1);
        }
        numberOfBytes = fread (buffer, 1, 1, clientFile);
    }


    return 0;
}
