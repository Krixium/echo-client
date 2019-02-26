/*---------------------------------------------------------------------------------------
--  SOURCE FILE:            main.c
--
--  PROGRAM:                echoc.out
--
--  FUNCTIONS:              
--                          void printHelp(const char *programName)
--                          void parseArguments(int argc, char *argv[])
--                          int setSocketTimeout(const size_t sec, const size_t usec, const int sock)
--                          void convertTime(size_t *ms, size_t *us, const struct timeval *time)
--                          int logStatsToFile(FILE *file, const struct timeval *start, const struct timeval *end, const size_t amount)
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

#include <getopt.h>

#define UWULIB_IMPL
#define UWULIBNET_IMPL

#include "uwuLib/uwuLib.h"
#include "uwuLib/uwuLibNet.h"

int instances;
char *address;
short port;
char *message;
int length;
int count;
int delay;

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

    parseArguments(argc, argv);

    // spawns instances - 1 children, parent handles last instance
    for (int i = 0; i < instances - 1; i++)
        if (fork() <= 0)
            break;

    // open file for logging
    sprintf(filename, "%s%d.log", argv[0], getpid());
    logFile = uwuOpenFile(filename, "w+");
    if (logFile == NULL)
    {
        perror("uwuOpenFile");
        return 1;
    }

    // allocate buffer
    if ((rcvBuffer = calloc(sizeof(char), length)) == NULL)
    {
        perror("calloc");
        return 1;
    }

    // make connection
    if (!uwuCreateConnectedSocket(&server, address, port))
    {
        perror("uwuCreateConnectedSocket");
        return 1;
    }

    // set timeout
    if (!setSocketTimeout(10, 0, server))
    {
        perror("setSocketTimeout");
        return 1;
    }

    // start echo loop
    for (int i = 0; i < count; i++)
    {
        // send
        amountSent = write(server, message, length);
        if (amountSent == 0)
        {
            perror("write");
            break;
        }

        // start receive timer
        if (gettimeofday(&start, NULL))
        {
            perror("gettimeofday");
            break;
        }

        // blocking read
        if (uwuReadAllFromSocket(server, rcvBuffer, length) <= 0)
        {
            perror("uwuReadAllFromSocket");
            break;
        }

        // stop receive timer
        if (gettimeofday(&end, NULL))
        {
            perror("gettimeofday");
            break;
        }

        // log
        if (logStatsToFile(logFile, &start, &end, amountSent) <= 0)
        {
            perror("logStatsToFile");
            break;
        }

        // wait if delay is specified
        if (delay > 0)
        {
            usleep(delay);
        }
    }

    fprintf(stdout, "Sending finished\n");
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
    fprintf(stderr, "Usage: %s -a [address] -p [port] -m [message] -l [length] -c [count] -d [delay] -n [instances]\n", programName);
    fprintf(stderr, "    -a - The address of the server.\n");
    fprintf(stderr, "    -p - The port of the server.\n");
    fprintf(stderr, "    -m - The message.\n");
    fprintf(stderr, "    -l - The length of the message - optional, defaults to length of message string.\n");
    fprintf(stderr, "    -c - The number of times each instance should send the message - optional, defaults to 1.\n");
    fprintf(stderr, "    -d - The amount of delay between each message in milliseconds - optional, defaults to 0.\n");
    fprintf(stderr, "    -n - The number of instances - optional, defaults to 1.\n");
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                parseArguments
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void parseArguments(int argc, char *argv[])
--                              int argc: The number of command line arguments.
--                              char *argvp[]: The command line arguments.
--
-- NOTES:
--                          Parses the command line arguments and sets the assocatied parameters.
--------------------------------------------------------------------------------------------------*/
void parseArguments(int argc, char *argv[])
{
    int c;

    // defaults
    instances = 1;
    address = NULL;
    port = 0;
    message = NULL;
    length = 0;
    count = 1;
    delay = 0;

    if (argc == 1)
    {
        printHelp(argv[0]);
        exit(1);
    }

    while ((c = getopt(argc, argv, "n:a:p:m:l:c:d:")) != -1)
    {
        switch (c)
        {
        case 'n':
            instances = atoi(optarg);
            break;
        case 'a':
            address = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'm':
            message = optarg;
            break;
        case 'l':
            length = atoi(optarg);
            break;
        case 'c':
            count = atoi(optarg);
            break;
        case 'd':
            delay = atoi(optarg);
            break;
        default:
            printHelp(argv[0]);
            exit(1);
        }
    }

    if (length == 0)
    {
        length = strlen(message);
    }

    if (length < strlen(message))
    {
        fprintf(stderr, "Length of packet must be greater than length of the message.");
        exit(1);
    }

    // check that all options have been given
    if (instances < 1 || address == NULL || port < 1024 || message == NULL || count < 1 || delay < 0)
    {
        printHelp(argv[0]);
        exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                setSocketTimeout
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int setSocketTimeout
--                              const size_t sec: The amount of seconds to wait before timing out.
--                              const size_t sec: The amount of nanosecond to wait before timing out.
--
-- RETURN:                  1 if the socket options were set, 0 otherwise.
--
-- NOTES:
--                          Sets the receive timeout and send timeout of a socket.
--------------------------------------------------------------------------------------------------*/
int setSocketTimeout(const size_t sec, const size_t usec, const int sock)
{
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        return 0;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        return 0;
    }

    return 1;
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                convertTime
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               void convertTime(size_t *ms, size_t *us, const struct timeval *time)
--                              size_t *ms: A pointer to store the milliseconds.
--                              size_t *us: A pointer to store the nanoseconds.
--                              const struct timeval *time: The time struct to convert.
--
-- NOTES:
--                          Converts a timeval struct to milliseconds.
--------------------------------------------------------------------------------------------------*/
void convertTime(size_t *ms, size_t *us, const struct timeval *time)
{
    *ms = time->tv_sec * 1000 + time->tv_usec / 1000;
    *us = time->tv_usec % 1000;
}

/*--------------------------------------------------------------------------------------------------
-- FUNCTION:                logStatsToFile
--
-- DATE:                    February 11, 2019
--
-- REVISIONS:               N/A
--
-- DESIGNER:                Benny Wang
--
-- PROGRAMMER:              Benny Wang
--
-- INTERFACE:               int logStatsToFile(FILE *file, const struct timeval *start, const struct timeval *end, const size_t amount)
--                              FILE *file: The log file.
--                              const struct timeval *start: The start time.
--                              const struct timeval *end: The stop time.
--                              const size_t amount: The amount of data.
--
-- RETURN:                  The number of bytes written to the log file.
--
-- NOTES:
--                          Logs the stats of a send to the log file. The format of the log is
--                          startimestamp(ms),endtimestamp(ms),delta(ms),amount(B).
--------------------------------------------------------------------------------------------------*/
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
