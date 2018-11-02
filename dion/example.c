#include <stdio.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
    FILE *file = fopen("input.txt","rb");
    int max_buf_len = 10;
    
    int read_done = 0;
    char buffer[max_buf_len];

    while (!read_done) {
        int buf_len = fread(buffer,1,max_buf_len,file);
        if (buf_len == max_buf_len) {
            char remain[1];
            int remain_len = fread(remain,1,1,file);
            if (remain_len == 0) {
                read_done = 1;
            }
            int remain_pointer = fseek(file,-1,SEEK_CUR);
        } else if (buf_len < max_buf_len) {
            read_done = 1;
        }
        for (int i=0; i<buf_len; i++) {
            printf("%c",buffer[i]);
        }
        printf("\n");
    }
}