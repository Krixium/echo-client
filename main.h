#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>

#include "uwuLib/uwuLib.h"
#include "uwuLib/uwuLibNet.h"

void printHelp(const char *programName);
void parseArguments(int argc, char *argv[]);

int setSocketTimeout(const size_t sec, const size_t usec, const int sock);

void convertTime(size_t *ms, size_t *us, const struct timeval *time);
int logStatsToFile(FILE *file, const struct timeval *start, const struct timeval *end, const size_t amount);

#endif // MAIN_H