/*
    Simple UDP Server
*/
 
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<string.h>
#include "Packet.h"
#include "Utility.h"
#include "stdbool.h"
 
#define BUFLEN 1035  //Max length of buffer
// #define PORT 8888   //The port on which to listen for incoming data
// #define FILEOUTPUTNAME "output.txt"

int writeFile(char *message, char *filename);
int getLFR(bool *packetReceived, int BUFFERSIZE);
int main(int argc, char * argv[])
{
    if(argc != 5) {
        perror("Command format = recvfile <filename> <windowsize> <buffersize> <port>");
        exit(1);
    }

    // initialize variable from command
    char * FILEOUTPUTNAME = argv[1];
    int WINDOWSIZE = charToInt(argv[2]);    
    int BUFFERSIZE = charToInt(argv[3]);
    int PORT = charToInt(argv[4]);
    Packet listOfPacket[BUFFERSIZE];
    bool packetReceived[BUFFERSIZE+1];
    int LAF=WINDOWSIZE-1,LFR=-1;

    printf("FILE NAME : %s\n", FILEOUTPUTNAME);
    printf("WINDOW SIZE : %d\n", WINDOWSIZE);
    printf("BUFFER SIZE : %d\n", BUFFERSIZE);
    printf("PORT : %d\n", PORT);

    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen , recv_len;
    char buf[BUFLEN];
    WSADATA wsa;
    ACK acknowledgement;
    Packet p;
    slen = sizeof(si_other) ;
    memset(packetReceived,'\0',BUFFERSIZE);
     
    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
     
    //Create a socket
    if((s = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
     
    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");
    writeFileInitiate(FILEOUTPUTNAME);
    //keep listening for data
    while(p.soh!=0x0)
    {
        printf("Waiting for data...");
        fflush(stdout);
         
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
         
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        p = parsePacket(buf);
        printf("soh: %x\n",p.soh);
        // printf("SeqNumber: %d\n",p.sequenceNumber);
        // printf("DataLength: %d\n",p.dataLength);
        // printf("Data:\n%s\n",p.data);
        // printf("checksum:%x\n",p.checksum);
        if (p.soh!=0x0 && p.checksum==checksum(p.data,p.dataLength) && p.sequenceNumber%BUFFERSIZE<=LAF && p.sequenceNumber%BUFFERSIZE>LFR){
            // writeFile(p.data,FILEOUTPUTNAME);
            listOfPacket[p.sequenceNumber%BUFFERSIZE] = p;
            packetReceived[p.sequenceNumber%BUFFERSIZE] = true;
            LFR = getLFR(packetReceived,BUFFERSIZE);
            if (LFR + WINDOWSIZE < BUFFERSIZE){
                LAF =  LFR + WINDOWSIZE;
            }else{
                LAF = BUFFERSIZE-1;
            }
            printf("seqnum:%d\n",p.sequenceNumber);
            printf("LFR:%d\nLAF:%d\n",LFR+(div(p.sequenceNumber,BUFFERSIZE).quot*BUFFERSIZE),LAF+(div(p.sequenceNumber,BUFFERSIZE).quot*BUFFERSIZE));
            memset(buf,'\0', BUFLEN);
            acknowledgement=createACK(LFR+(div(p.sequenceNumber,BUFFERSIZE).quot*BUFFERSIZE)+1);
            printf("ack: %c\nnextSequenceNumber: %d\nchecksum: %x\n",acknowledgement.ack,acknowledgement.nextSequenceNumber,acknowledgement.checksum);
            ackToString(acknowledgement,buf);
            //now reply the client with the same data
            if (sendto(s, buf, 6, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
            {
                printf("sendto() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }   
        }
        if (LFR == BUFFERSIZE-1){
             for (int i=0;i<BUFFERSIZE;i++){
                // printf("writing frame %d\n",i);
                writeFile(listOfPacket[i].data,FILEOUTPUTNAME);
            }
            memset(packetReceived,false,BUFFERSIZE);
            LFR = -1;
            LAF = WINDOWSIZE-1;
        }
    }
    // printf("------------------------------------------------------------------------\n");
    for (int j=0;j<=LFR;j++){
        // printf("writing frame %d\n",j);
        writeFile(listOfPacket[j].data,FILEOUTPUTNAME);
    }
    closesocket(s);
    WSACleanup();
     
    return 0;
}
int getLFR(bool *packetReceived, int BUFFERSIZE){
    int i;
    for (i=BUFFERSIZE-1;i>=0;i--){
        if (packetReceived[i]==true){
            return i;
        }
    }
}