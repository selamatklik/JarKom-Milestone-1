#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readFile(char *message,char *filename, int BUFLEN);
int writeFile(char *message, char *filename, int BUFLEN);
int charToInt(char * c);

#endif