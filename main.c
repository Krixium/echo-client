#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "net.h"

int main(int argc, char *argv[])
{
    // network
    int server;
    char *rcvBuffer;

    // arguements
    char *address;
    short port;
    char *message;
    unsigned int length;
    unsigned int count = 1;
    unsigned int delay = 0;

    // check for wrong arguement count
    if (argc < 5 || argc > 7)
    {
        printHelp(argv[0]);
        return 0;
    }

    // grab required arguements
    address = argv[1];
    port = atoi(argv[2]);
    message = argv[3];
    length = atoi(argv[4]);

    // get count if specified
    if (argc >= 6)
    {
        count = atoi(argv[5]);
    }

    // get delay if specified
    if (argc == 7)
    {
        delay = atoi(argv[6]);
    }

    // make connection
    if (!createConnectedSocket(&server, address, port))
    {
        perror("Could not connect to server");
        return 1;
    }

    // allocate buffer space
    rcvBuffer = calloc(sizeof(char), length);

    // start echo loop
    printf("Starting loop ...\n");
    for (int i = 0; i < count; i++)
    {
        printf("Sending %s to %s ...\n", message, address);
        sendAndListen(server, message, rcvBuffer, length);
        printf("Received %s from %s\n", message, address);
    }

    // free buffer
    free(rcvBuffer);

    return 0;
}

void printHelp(const char *programName)
{
    printf("Usage: %s address port message length [count] [delay]\n", programName);
}