/*
    Simple udp client
*/
#include<stdio.h>
#include<winsock2.h>
#include<string.h>
#include"Packet.h"
#include "Utility.h"
 
// #define SERVER "127.0.0.1"  //ip address of udp server
// #define BUFLEN 1034  //Max length of buffer
// #define PORT 8888   //The port on which to listen for incoming data

// int readFile(char *message,char *filename, int BUFLEN);
// int charToInt(char * c);

int main(int argc, char * argv[])
{
    if(argc != 6) {
        perror("Command format = sendfile <filename> <windowsize> <buffersize> <destination_ip> <destination_port>");
        exit(1);
    }

    // initialize variable from command
    char * FILENAME = argv[1];
    int WINDOWSIZE = charToInt(argv[2]);    
    int BUFLEN = charToInt(argv[3]);
    char * SERVER = argv[4];
    int PORT = charToInt(argv[5]);

    printf("FILE NAME : %s\n", FILENAME);
    printf("WINDOW SIZE : %d\n", WINDOWSIZE);
    printf("BUFFER SIZE : %d\n", BUFLEN);
    printf("DESTINATION IP : %s\n", SERVER);
    printf("DESTINATION PORT : %d\n", PORT);

    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
    char filename[BUFLEN];
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
     
    //start communication
    while(1)
    {
        printf("Enter filename : ");
        scanf("%s",filename);
        memset(message,'\0',BUFLEN);
        readFile(message,filename, BUFLEN);
        p=createPacket(message,1);
        printf("soh: %x\n",p.soh);
        printf("SeqNumber: %d\n",p.sequenceNumber);
        printf("DataLength: %d\n",p.dataLength);
        printf("Data:\n%s\n",p.data);
        printf("checksum:%x\n",p.checksum);
        memset(message,'\0',BUFLEN);
        packetToString(p,message);
        //send the message
        if (sendto(s, message, BUFLEN , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
         
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        acknowledgement = parseACK(buf);
        printf("ack: %c\nnextSequenceNumber: %d\nchecksum: %x\n",acknowledgement.ack,acknowledgement.nextSequenceNumber,acknowledgement.checksum);

    }
 
    closesocket(s);
    WSACleanup();
 
    return 0;
}