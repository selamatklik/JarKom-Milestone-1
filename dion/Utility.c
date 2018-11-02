#include "Utility.h"

int readFile(char *message, char *filename) {
    FILE *file = fopen(filename,"r");
    char c = fgetc(file);
    int i = 0;
    while (c != EOF){
        message[i] = c;
        i+=1;
        c=fgetc(file);
    }
    printf("file succesfully read\n");
    fclose(file);
}

void read_file(char* message, char *file_name) {
	if (access(file_name, F_OK) == -1) {
		printf("File not found\n");
		exit(EXIT_FAILURE);
	} else {
		FILE *file = fopen(file_name,"r");
		char c = fgetc(file); int i=0;
		while (c != EOF && i < BUFFER_LEN) {
			message[i] = c;
			i++; c = fgetc(file);
		}

		printf("Read file completed\n");
		fclose(file);
	}
}

int writeFile(char *message, char *filename, int BUFLEN){
    FILE *file = fopen(filename,"w");
    int i = 0;
    while (message[i]!='\0' && i < BUFLEN){
        fputc(message[i],file);
        i+=1;
    }
    fclose(file);
}

int charToInt(char * c) {
    int i, num = 0;
    int size = strlen(c);
    for(i=0; i<size; i++){
        num = num*10 + (c[i]-'0'); 
    }
    return num;
}