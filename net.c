#include "net.h"

#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

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

void sendAndListen(const int sock, const char *message, char *buffer, const unsigned int length)
{
    send(sock, message, length, 0);
    readAllFromSocket(sock, buffer, length);
}

void readAllFromSocket(const int sock, char *buffer, const unsigned int size)
{
    int n = 0;
    char *bufferPointer = buffer;
    unsigned int remaining = size;

    while ((n = recv(sock, bufferPointer, remaining, 0)) < size)
    {
        bufferPointer += n;
        remaining -= n;
    }
}