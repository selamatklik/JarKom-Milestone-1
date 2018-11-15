#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readFile(char *message,char *filename, int BUFLEN);
int writeFileInitiate(char *filename);
int writeFile(char *message, char *filename);
int readFileCont(char * message, char * filename, int BUFLEN, int readCount);
int charToInt(char * c);
int writeFileLen(char *message, char *filename, int len);

#endif