/*
    Simple UDP Server
*/
 
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<string.h>
#include"Packet.h"
 
#define BUFLEN 1034  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
#define FILEOUTPUTNAME "output.txt"

int writeFile(char *message, char *filename);
int main()
{
    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen , recv_len;
    char buf[BUFLEN];
    WSADATA wsa;
    ACK acknowledgement;
    Packet p;
    slen = sizeof(si_other) ;
     
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
 
    //keep listening for data
    while(1)
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
        printf("SeqNumber: %d\n",p.sequenceNumber);
        printf("DataLength: %d\n",p.dataLength);
        printf("Data:\n%s\n",p.data);
        printf("checksum:%x\n",p.checksum);
        writeFile(p.data,FILEOUTPUTNAME);
        memset(buf,'\0', BUFLEN);
        acknowledgement=createACK(p.sequenceNumber+1);
        printf("ack: %c\nnextSequenceNumber: %d\nchecksum: %x\n",acknowledgement.ack,acknowledgement.nextSequenceNumber,acknowledgement.checksum);
        ackToString(acknowledgement,buf);
        //now reply the client with the same data
        if (sendto(s, buf, 6, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }
 
    closesocket(s);
    WSACleanup();
     
    return 0;
}
int writeFile(char *message, char *filename){
    FILE *file = fopen(filename,"w");
    int i = 0;
    while (message[i]!='\0' && i < BUFLEN){
        fputc(message[i],file);
        i+=1;
    }
    fclose(file);
}