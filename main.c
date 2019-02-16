/*---------------------------------------------------------------------------------------
--  SOURCE FILE:            main.c
--
--  PROGRAM:                echoc.out
--
--  FUNCTIONS:              
--                          void printHelp(const char *programName);
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
--                          The main file of the application.
---------------------------------------------------------------------------------------*/
#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "uwuLib/uwuLib.h"
#include "uwuLib/uwuLibNet.h"

#define UWULIB_IMPL
#define UWULIBNET_IMPL

#include "uwuLib/uwuLib.h"
#include "uwuLib/uwuLibNet.h"

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                main
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int main(int argc, char *argv[])
--                              argc: The number of command line arguements.
--                              argv: The command line arguements.
--
-- RETURNS:                 0 if the program exits without error, 1 otherwise.
--
-- NOTES:
--                          The entry point of the program as well as the main body. Parses the
--                          command line arguements extract the required values for sending a TCP
--                          packet to a server and waits for the echo response. If specified, the
--                          program will loop through the processes of creating a socket, sending
--                          a message, wait for a response, and close the socket.
--------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    // book keeping
    char filename[256];
    FILE *logFile;
    size_t amountSent = 0;
    size_t numSent = 0;
    size_t totalDataSent = 0;
    struct timeval start;
    struct timeval end;

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
        delay = atoi(argv[6]) * 1000;
    }

    // open file for logging
    sprintf(filename, "%s-%d.log", argv[0], getpid());
    logFile = uwuOpenFile(filename, "w+");
    if (logFile == NULL)
    {
        perror("Could not create log file.");
    }

    // allocate buffer space
    rcvBuffer = calloc(sizeof(char), length);

    if (rcvBuffer == NULL)
    {
        perror("Could not allocate memory.");
        return 1;
    }

    // make connection
    if (!uwuCreateConnectedSocket(&server, address, port))
    {
        perror("Could not connect to server");
        return 1;
    }

    // start echo loop
    printf("Starting loop ...\n");
    for (int i = 0; i < count; i++)
    {
        if (gettimeofday(&start, NULL))
        {
            perror("Getting timestamp failed");
            return 1;
        }

        printf("Sending %s to %s ...\n", message, address);
        amountSent = write(server, message, length);
        if (amountSent == 0)
        {
            perror("Could not send message");
            return 1;
        }
        // book keeping
        numSent++;
        totalDataSent += amountSent;

        bzero(rcvBuffer, length);
        if (uwuReadAllFromSocket(server, rcvBuffer, length) == 0)
        {
            perror("Nothing was received from server");
            return 1;
        }
        printf("Received %s from %s\n", rcvBuffer, address);

        if (gettimeofday(&end, NULL))
        {
            perror("Getting timestamp failed");
            return 1;
        }

        if (uwuLogDeltaTimeToFile(logFile, &start, &end) == 0)
        {
            perror("Could not write time to file.");
            return 1;
        }

        if (delay > 0)
        {
            usleep(delay);
        }
    }

    // clean up
    close(server);
    free(rcvBuffer);

    // Print results
    fprintf(logFile, "Sending finished\n");
    fprintf(logFile, "    Packet Size:        %d\n", length);
    fprintf(logFile, "    Packets Sent:       %ld\n", numSent);
    fprintf(logFile, "    Total Data Sent:    %ld\n", totalDataSent);

    uwuCloseFile(&logFile);

    return 0;
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                printHelp
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void printHelp(cosnt char *programName)
--                              const char *programName: The name of the program a.k.a argv[0].
--
-- NOTES:
--                          Prints a help message when the program is ran with the incorrect command
--                          line arguements.
--------------------------------------------------------------------------------------------------*/
void printHelp(const char *programName)
{
    printf("Usage: %s address port message length [count] [delay]\n", programName);
}