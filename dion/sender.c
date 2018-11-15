#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <sys/time.h>
#include <pthread.h>

#include "Packet.h"

#define PACKETLEN 1035
#define MAXDATA 1024

int socket_fd;
void *receiveAck() {
    while(1) {
        int recv_len;
        if (recv_len = recvfrom(socket_fd, buffer, BUFLEN, 0, (struct sockaddr *) &clientAddress, &s_len) == -1){
            printf("Error in receiving packet\n");
            exit(1);
        } else {
            printf("Packet received\n");
        }
    }
}

int main(int argc, char* argv[]) {
    
    // check argument input
    if (argc != 6) {
        perror("Command format: ./sendfile <filename> <windowsize> <buffersize> <destination_ip> <destination_port>");
		exit(1);    
    } 

    char* fileInputName = argv[1];
    int windowSize = atoi(argv[2]);
    int bufferSize = atoi(argv[3]);
    char* server = argv[4];
    int port = atoi(argv[5]);

    printf("File name: %s\n", fileInputName);
    printf("Window size: %d\n", windowSize);
    printf("Buffer size: %d\n", bufferSize);
    printf("Destination IP: %s\n", server);
    printf("Destination port: %d\n", port);

    // create socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed\n");
        exit(1);
    } else {
        printf("Socket creation success\n");
    }

    // fill client info
    struct sockaddr_in serverAddress;
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    struct hostent *destHostnet = gethostbyname(server);
    bcopy(destHostnet->h_addr, (char*)&serverAddress.sin_addr, destHostnet->h_length);

    FILE *fileInput = fopen(fileInputName,"rb");
    int maxBufLen = MAXDATA * bufferSize;
    char buffer[maxBufLen + 1];

    int finishRead = 0;
    int slidingWindowSize = windowSize * MAXDATA;

    int lastAckReceived = 0;
    int lastFrameSent = lastAckReceived + windowSize;

    pthread_t t;
    int x = pthread_create(&t,NULL,receiveAck,NULL);

    while (!finishRead) {
        memset(buffer,0,sizeof(buffer));
        int bufLen = fread(buffer,1,maxBufLen,fileInput);

        if (bufLen == 0) {
            break;
        } else if (bufLen < maxBufLen) {
            finishRead = 1;
        }
        
        for (int i=lastAckReceived; i<=lastFrameSent; i++) {
            char message[MAXDATA];
            memset(message,'\0',sizeof(message));

            int startMessage = i * MAXDATA;
            int endMessage = startMessage + MAXDATA;
            if (endMessage-startMessage > bufLen) {
                endMessage = startMessage + bufLen;
            }

            int j;
            for (j=startMessage; j<endMessage; j++) {
                message[j-startMessage] = buffer[j];
            }

            printf("%s\n",message);

            // create packet
            Packet p = createPacket(message,i);
            // printf("Packet\n");
            // printf("soh: %x\n",p.soh);
            // printf("SeqNumber: %d\n",p.sequenceNumber);
            // printf("DataLength: %d\n",p.dataLength);
            // printf("Data:\n%s\n",p.data);
            printf("checksum:%x\n",p.checksum);

            if (p.dataLength > 0) {
                char strPacket[PACKETLEN];
                memset(strPacket,0,sizeof(strPacket));
                packetToString(p,strPacket);
            
                sendto(socket_fd,strPacket,PACKETLEN,0, (struct sockaddr*) &serverAddress, sizeof(serverAddress));   
            }
        }
        lastAckReceived++;
    }

    Packet p = createPacket("",0);
    p.soh = 0x0;
    char strPacket[PACKETLEN + 1];
    memset(strPacket,0,sizeof(strPacket));
    packetToString(p,strPacket);
    sendto(socket_fd,strPacket, PACKETLEN, 0, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    close(socket_fd);
    return 0;
}