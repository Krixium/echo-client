/*---------------------------------------------------------------------------------------
--  SOURCE FILE:            net.c
--
--  PROGRAM:                echoc.out
--
--  FUNCTIONS:              
--                          int createConnectedSocket(int *sock, const char* address, const short port)
--                          void sendAndListen(const int sock, const char* message, char *buffer, const unsigned int length)
--                          void readAllFromSocket(const int sock, char *buffer, const unsigned int size)
--
--  DATE:                   February 11, 2019
--
--  REVISIONS:              N/A
--
--  DESIGNERS:              Benny Wang
--
--  PROGRAMMERS:            Benny Wang
--
--  NOTES:
--                          All functions that are involved with the networking aspect of this application.
---------------------------------------------------------------------------------------*/
#include "net.h"

#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                createConnectedSocket
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int createConnectedSocket(int *sock, const char *address, const short port)
--                              int *sock: The pointer that will hold the connected socket.
--                              const char *address: The address of the server.
--                              const short port: The port the server is listening on.
--
-- RETURNS:                 1 if the socket was created and connected without error, 0 otherwise.
--
-- NOTES:
--                          Creates a connected TCP socket to address:port and stores the newly
--                          connected socket to sock. The newly created socket is set to reuse
--                          in case of a crash.
--------------------------------------------------------------------------------------------------*/
int createConnectedSocket(int *sock, const char *address, const short port)
{
    int arg = 1;
    struct hostent *host;
    struct sockaddr_in server;

    if ((host = gethostbyname(address)) == NULL)
    {
        return 0;
    }

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        return 0;
    }

    if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1)
    {
        return 0;
    }

    bzero(&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    bcopy(host->h_addr, (char *)&server.sin_addr, host->h_length);

    if (connect(*sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        return 0;
    }

    return 1;
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                sendAndListen
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void sendAndListen(const int sock, const char *message, char *buffer, const unsigned int length)
--                              const int sock: The socket to send and listen on.
--                              const char *message: The message to send.
--                              char *buffer: The buffer to store the received message.
--                              const unsigned int length: The length of buffer.
--
-- NOTES:
--                          Sends a message on sock and then reads length bytes to buffer from socket.
--------------------------------------------------------------------------------------------------*/
void sendAndListen(const int sock, const char *message, char *buffer, const unsigned int length)
{
    send(sock, message, length, 0);
    readAllFromSocket(sock, buffer, length);
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                readAllFromSocket
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void readAllFromSocket(const int sock, char *buffer, const unsigned int size)
--                              const int sock: The socket to read from.
--                              char *buffer: The buffer to store the read bytes.
--                              const unsigned int size: The size of the buffer.
--
-- NOTES:
--                          Reads size bytes from sock into buffer.
--------------------------------------------------------------------------------------------------*/
void readAllFromSocket(const int sock, char *buffer, const unsigned int size)
{
    int n = 1;
    char *bufferPointer = buffer;
    unsigned int remaining = size;

    while (n > 0 && remaining > 0)
    {
        n = recv(sock, bufferPointer, remaining, 0);
        bufferPointer += n;
        remaining -= n;
    }
}