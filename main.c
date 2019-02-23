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
    struct timeval start;
    struct timeval end;

    // network
    int server;
    char *rcvBuffer;

    // arguements
    short instances;
    char *address;
    short port;
    char *message;
    unsigned int length;
    unsigned int count = 1;
    unsigned int delay = 0;

    // check for wrong arguement count
    if (argc < 5 || argc > 8)
    {
        printHelp(argv[0]);
        return 0;
    }

    // grab required arguements
    instances = atoi(argv[1]);
    address = argv[2];
    port = atoi(argv[3]);
    message = argv[4];
    length = atoi(argv[5]);

    // get count if specified
    if (argc == 7)
    {
        count = atoi(argv[6]);
    }

    // get delay if specified
    if (argc == 8)
    {
        count = atoi(argv[6]);
        delay = atoi(argv[7]) * 1000;
    }

    // spawn instances - 1 children because parent servers as the last instances
    for (int i = 0; i < instances - 1; i++)
    {
        if (fork() <= 0)
        {
            break;
        }
    }

    // open file for logging
    sprintf(filename, "%s%d.log", argv[0], getpid());
    logFile = uwuOpenFile(filename, "w+");
    if (logFile == NULL)
    {
        perror("Could not create log file");
        return 1;
    }

    // write header to file
    if (fprintf(logFile, "start(ms),end(ms),delta(ms),amount(B)\n") <= 0)
    {
        perror("Could not write to file");
        return 1;
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
    for (int i = 0; i < count; i++)
    {
        // send
        amountSent = write(server, message, length);
        if (amountSent == 0)
        {
            perror("Could not send message");
            break;
        }

        // start receive timer
        if (gettimeofday(&start, NULL))
        {
            perror("Getting timestamp failed");
            break;
        }

        // blocking read
        if (uwuReadAllFromSocket(server, rcvBuffer, length) <= 0)
        {
            perror("Nothing was received from server");
            break;
        }

        // stop receive timer
        if (gettimeofday(&end, NULL))
        {
            perror("Getting timestamp failed");
            break;
        }

        // log
        if (logStatsToFile(logFile, &start, &end, amountSent) <= 0)
        {
            perror("Could not write time to file");
            break;
        }

        // wait if delay is specified
        if (delay > 0)
        {
            usleep(delay);
        }
    }
    fprintf(stdout, "Sending finished\n");

    // clean up
    close(server);
    free(rcvBuffer);

    fflush(logFile);
    fclose(logFile);

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
    printf("Usage: %s instances address port message length [count] [delay]\n", programName);
}

void convertTime(size_t *ms, size_t *us, const struct timeval *time)
{
    *ms = time->tv_sec * 1000 + time->tv_usec / 1000;
    *us = time->tv_usec % 1000;
}

int logStatsToFile(FILE *file, const struct timeval *start, const struct timeval *end, const size_t amount)
{
    size_t msStart;
    size_t usStart;
    size_t msEnd;
    size_t usEnd;
    size_t msDelta;
    size_t usDelta;
    struct timeval delta;

    timersub(end, start, &delta);

    convertTime(&msStart, &usStart, start);
    convertTime(&msEnd, &usEnd, end);
    convertTime(&msDelta, &usDelta, &delta);

    return fprintf(file, "%lu.%03lu,%lu.%03lu,%lu.%03lu,%lu\n", msStart, usStart, msEnd, usEnd, msDelta, usDelta, amount);
}