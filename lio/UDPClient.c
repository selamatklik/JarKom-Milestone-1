/*
    Simple udp client
*/
#include<stdio.h>
#include<winsock2.h>
#include<string.h>
#include<time.h>
#include "Packet.h"
#include "Utility.h"
 
// #define SERVER "127.0.0.1"  //ip address of udp server
#define PACKETLEN 1035
#define MAXDATA 1024  //Max length of buffer
// #define PORT 8888   //The port on which to listen for incoming data

// int readFile(char *message,char *filename, int PACKETLEN);
// int charToInt(char * c);
int sock_fd, n, len, dest_port, buffer_size;
struct sockaddr_in server_address; 
struct hostent *dest_hostnet;

int main(int argc, char * argv[])
{
    if(argc != 6) {
        perror("Command format = sendfile <filename> <windowsize> <buffersize> <destination_ip> <destination_port>");
        exit(1);
    }
    // initialize variable from command
    char * FILENAME = argv[1];
    int WINDOWSIZE = charToInt(argv[2]);    
    int BUFFERSIZE = charToInt(argv[3]);
    char * SERVER = argv[4];
    int PORT = charToInt(argv[5]);

    printf("FILE NAME : %s\n", FILENAME);
    printf("WINDOW SIZE : %d\n", WINDOWSIZE);
    printf("BUFFER SIZE : %d\n", BUFFERSIZE);
    printf("DESTINATION IP : %s\n", SERVER);
    printf("DESTINATION PORT : %d\n", PORT);

    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);
    char buf[PACKETLEN];
    char message[PACKETLEN];
    char filename[PACKETLEN];
    char bufferFileInput[MAXDATA*BUFFERSIZE + 1];
    WSADATA wsa;
    Packet p;
    ACK acknowledgement;
 
    // Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
     
    //create socket
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        printf("socket() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    //setup address structure
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

    int finish_read = 0;

    // FILE *file = fopen(FILENAME,"r");
    // while (!finish_read){
    //     int buf_size = fread(buf, BUFFERSIZE * MAXDATA,file);
    //     printf("buf size: %d buffersize: %d, maxdata: %d\n", buf_size, BUFFERSIZE, MAXDATA);
    //     if (buf_size == BUFFERSIZE * MAXDATA) {
    //         char remain[1];
    //         int remain_len = fread(remain,1,1,file);
    //         if (remain_len == 0) {
    //             finish_read = 1;
    //         }
    //         int remain_ptr = fseek(file,-1,SEEK_CUR);
    //     } else if (buf_size < BUFFERSIZE * MAXDATA) {
    //         finish_read = 1;
    //     }

    //     int ack_state[WINDOWSIZE + 1], frame_send[WINDOWSIZE + 1];
    //     memset(ack_state,0,sizeof(ack_state));
    //     memset(frame_send,0,sizeof(frame_send));

    //     unsigned long frame_time[WINDOWSIZE + 1];
    //     for (int i=0; i<WINDOWSIZE; i++) {
    //         struct timeval t;
    //         mingw_gettimeofday(&t,NULL);
    //         frame_time[i] = t.tv_sec;
    //     }

    //     int last_ack_rcv = -1;
    //     int last_frame_send = last_ack_rcv + WINDOWSIZE;

    //     int finish_send = 0, nb_frame = 10;
    //     while (!finish_send) {
    //         // count step to slide
    //         int nb_step = 1;
    //         if (ack_state[0]) {
    //             for (int i=1; i<WINDOWSIZE; i++) {
    //                 if (!ack_state[i]) break;
    //                 nb_step++;
    //             }
    //         } 

    //         // slide nb_step step
    //         for (int i=0; i<WINDOWSIZE - nb_step; i++) {
    //             ack_state[i] = ack_state[i+nb_step];
    //             frame_send[i] = frame_send[i+nb_step];
    //             frame_time[i] = frame_time[i+nb_step];
    //         }
    //         for (int i=WINDOWSIZE-nb_step; i<WINDOWSIZE; i++) {
    //             ack_state[i] = 0;
    //             frame_send[i] = 0;
    //         }
    //         last_ack_rcv += nb_step;
    //         last_frame_send = last_ack_rcv + WINDOWSIZE;

    //         // send message
    //         for (int i=0; i<WINDOWSIZE; i++) {
    //             int frame_number = last_ack_rcv + i + 1;
    //             // printf("frame number : %d nb_frame : %d\n",frame_number,nb_frame);
    //             if (frame_number < nb_frame) {
                    
    //                 int cur_buf_ptr = frame_number * MAXDATA;
    //                 int cur_data_len = MAXDATA;
    //                 // if (buf_size - cur_buf_ptr < MAXDATA) {
    //                 //     cur_data_len = buf_size - cur_buf_ptr;
    //                 // }
                    
    //                 // printf("%d %d\n", cur_buf_ptr, cur_data_len);
    //                 char data[MAXDATA];
    //                 memcpy(data, buf+cur_buf_ptr, cur_data_len);

    //                 struct timeval now;
    //                 unsigned long diff_time = now.tv_sec - frame_time[i];
    //                 int loss = !ack_state[i] && diff_time > 1000;
                    
    //                 // printf("%d %d\n",!frame_send[i],loss);
    //                 if (!frame_send[i] || loss) {
    //                     Packet packet = createPacket(buf,WINDOWSIZE);
    //                     char str_packet[PACKETLEN+1];

    //                     memset(str_packet,0,sizeof(str_packet));

    //                     //send the message
    //                     if (sendto(s, str_packet, PACKETLEN+1, 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR){
    //                         printf("sendto() failed with error code : %d" , WSAGetLastError());
    //                         exit(EXIT_FAILURE);
    //                     }
    //                 }
    //             }
    //         }

    //         // receive ack

    //     }
    // }   

    int readCount = 0;
    int flag = 0;
    int SWS = 0;
    memset(bufferFileInput,'\0', MAXDATA*BUFFERSIZE + 1);

    while(!flag){
        flag = readFileCont(bufferFileInput, FILENAME, MAXDATA*BUFFERSIZE + 1, readCount);
        printf("%d. %s\n\n",readCount,bufferFileInput);
        int thisSWS = 0;
        while(strlen(bufferFileInput) > 0) {
            if (thisSWS > 0 && (thisSWS)*MAXDATA >= strlen(bufferFileInput)){
                break;
            }
            printf("thisSWS: %d sws: %d windowsize: %d\n", thisSWS, SWS, WINDOWSIZE);
            int idx;
            memset(message,'\0',PACKETLEN);
            for(idx = 0; idx<MAXDATA && idx < strlen(bufferFileInput) ; idx++){
                message[idx] = bufferFileInput[MAXDATA*thisSWS+idx];
            }
            // printf("message: %s\n",message);
            p = createPacket(message,SWS);
            printf("Packet\n");
            printf("soh: %x\n",p.soh);
            printf("SeqNumber: %d\n",p.sequenceNumber);
            printf("DataLength: %d\n",p.dataLength);
            printf("Data:\n%s\n",p.data);
            printf("checksum:%x\n",p.checksum);
            memset(message,'\0',PACKETLEN);
            packetToString(p, message);

            //send the message
            if (sendto(s, message, PACKETLEN , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
            {
                printf("sendto() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }
            
            //receive a reply and print it
            //clear the buffer by filling null, it might have previously received data
            memset(buf,'\0', PACKETLEN);
            //try to receive some data, this is a blocking call
            if (recvfrom(s, buf, PACKETLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
            {
                printf("recvfrom() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }
            acknowledgement = parseACK(buf);
            printf("ACK\nack: %c\nnextSequenceNumber: %d\nchecksum: %x\n",acknowledgement.ack,acknowledgement.nextSequenceNumber,acknowledgement.checksum);
            thisSWS++;
            SWS++;
        }

        memset(bufferFileInput,'\0',strlen(bufferFileInput));
        readCount++;
    }
    p = createPacket("",SWS);
    p.soh = 0x0;
    memset(message,'\0',PACKETLEN);
    packetToString(p,message);
    if (sendto(s, message, PACKETLEN , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    
    // readFile(bufferFileInput,FILENAME, MAXDATA * BUFFERSIZE);
    // // printf("input:%s\n",bufferFileInput);
    
    //start communication
 
    closesocket(s);
    WSACleanup();
 
    return 0;
}