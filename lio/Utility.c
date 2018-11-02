#include "Utility.h"

int readFile(char *message,char *filename, int BUFLEN){
    FILE *file = fopen(filename,"r");
    char c = fgetc(file);
    int i = 0;
    while (c!=EOF && i < BUFLEN){
        message[i] = c;
        i+=1;
        c=fgetc(file);
    }
    printf("file succesfully read\n");
    fclose(file);
}

int writeFileInitiate(char *filename){
    FILE *file = fopen(filename,"w");
    fclose(file);
}
int writeFile(char *message, char *filename){
    FILE *file = fopen(filename,"a");
    int i = 0;
    while (message[i]!='\0'){
        fputc(message[i],file);
        i++;
    }
    fclose(file);
}

int readFileCont(char *message, char *filename, int BUFLEN, int readCount){
    FILE *file = fopen(filename,"r");
    fseek(file, readCount*BUFLEN, SEEK_SET );
    char c = fgetc(file);
    int i = 0;
    while (c!=EOF && i < BUFLEN){
        message[i] = c;
        i++;
        c=fgetc(file);
    }
    message[i] = '\0';

    printf("%d. read %d character\n", readCount,i);
    fclose(file);
    
    if(i<BUFLEN){
        return 1;
    } else {
        return 0;
    }
    
}

int charToInt(char * c) {
    int i, num = 0;
    int size = strlen(c);
    for(i=0; i<size; i++){
        num = num*10 + (c[i]-'0'); 
    }
    return num;
}