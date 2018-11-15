#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include "Packet.h"

#define MAXDATA 1024
#define BUFLEN 1035

int getLFR(int* packetReceived, int BUFFERSIZE){
    int i;
    for (i = BUFFERSIZE-1; i>=0; i--){
        if (packetReceived[i] == 1){
            return i;
        }
    }
}

int main(int argc, char* argv[]) { 

    // check input format is valid
    if(argc != 5) {
        perror("Command format = recvfile <filename> <windowsize> <buffersize> <port>\n");
        exit(1);
    }

    // get argument value
    char* fileOutputName = argv[1];
    int windowSize = atoi(argv[2]);
    int bufferSize = atoi(argv[3]) * MAXDATA;
    int port = atoi(argv[4]);

    // display input argument
    printf("File name: %s\n", fileOutputName);
    printf("Window size: %d\n", windowSize);
    printf("Buffer size: %d\n", bufferSize);
    printf("Port: %d\n", port);

    // initialize main variable
    Packet listOfPacket[bufferSize + 1];
    int packetReceived[bufferSize + 1];
    int lastAcceptedFrame = windowSize - 1;
    int lastFrameReceived = 0;

    // create socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed\n");
        exit(1);
    } else {
        printf("Socket creation success\n");
    }

    // fill server info
    struct sockaddr_in serverAddress, clientAddress;
    memset(&serverAddress,0,sizeof(serverAddress));
    memset(&clientAddress,0,sizeof(clientAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // bind socket
    int bindStatus = bind(socket_fd, (const struct sockaddr*) &serverAddress, sizeof(serverAddress));
    if (bindStatus < 0) {
        perror("Socket bind to server failed\n");
        exit(1);
    } else {
        printf("Socket bind to server success\n");
    }

    // copast from lio
    writeFileInitiate(fileOutputName);
    Packet p;
    ACK ack;
    char buffer[BUFLEN];

    // receive until last packet
    while (p.soh != 0x0) {
        printf("Waiting for data...\n");
    
        // clear buffer 
        memset(buffer,'\0',BUFLEN);

        int recv_len;
        int s_len = sizeof(clientAddress);
        
        if (recv_len = recvfrom(socket_fd, buffer, BUFLEN, 0, (struct sockaddr *) &clientAddress, &s_len) == -1){
            printf("Error in receiving packet\n");
            exit(1);
        } else {
            printf("Packet received\n");
        }

        // parse buffer to packet
        p = parsePacket(buffer);
        printf("Soh: %x\n",p.soh);
        if (p.soh == 0x0) {
            break;
        }
        // printf("Data: %s\n",p.data);

        int lastPacket = p.soh == 0x0;
        int finePacket = p.checksum == checksum(p.data,p.dataLength);
        int acceptablePacket = p.sequenceNumber%bufferSize <= lastAcceptedFrame;

        if (!lastPacket && finePacket && acceptablePacket) {
            listOfPacket[p.sequenceNumber%bufferSize] = p;
            packetReceived[p.sequenceNumber%bufferSize] = 1;
            lastFrameReceived = getLFR(packetReceived, bufferSize);

            if (lastFrameReceived + windowSize < bufferSize) {
                lastAcceptedFrame = lastFrameReceived + windowSize;
            } else {
                lastAcceptedFrame = bufferSize - 1;
            }

            // display packet received info
            printf("Sequence number: %d\n", p.sequenceNumber);
            printf("Last frame received: %d\n", lastFrameReceived + (div(p.sequenceNumber,bufferSize).quot * bufferSize));
            printf("Last accepted frame: %d\n", lastAcceptedFrame+(div(p.sequenceNumber,bufferSize).quot*bufferSize));

            memset(buffer,'\0',BUFLEN);

            ack = createACK(lastFrameReceived+(div(p.sequenceNumber,bufferSize).quot*bufferSize)+1);
            printf("ACK: %c\nnextSequenceNumber: %d\nchecksum: %x\n",ack.ack,ack.nextSequenceNumber,ack.checksum);
            ackToString(ack,buffer);

            sendto(socket_fd,buffer,6,0,(struct sockaddr*) &clientAddress, sizeof(clientAddress));
        }

        if (lastFrameReceived == bufferSize - 1) {
            for (int i=0; i<bufferSize; i++) {
                // printf("%s\n",listOfPacket[i].data);
                writeFileLen(listOfPacket[i].data, fileOutputName, listOfPacket[i].dataLength);
            }
            memset(packetReceived,0,bufferSize);
            lastAcceptedFrame = 0;
            lastFrameReceived = windowSize - 1;
        }
    }

    for (int i=0; i<=lastFrameReceived; i++) {
        // printf("%s",listOfPacket[i].data);
        writeFileLen(listOfPacket[i].data, fileOutputName, listOfPacket[i].dataLength);
    }

    close(socket_fd);
    return 0;
}


// int sock_fd, len, window_size, buffer_size, port;
// struct sockaddr_in client_address;

// void create_socket() {
//     sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sock_fd < 0) {
//         perror("socket creation failed");
//         exit(EXIT_FAILURE);
//     }
// }

// void fill_server_info() {
//     memset(&server_address,0,sizeof(server_address));
//     server_address.sin_family = AF_INET;
//     server_address.sin_addr.s_addr = INADDR_ANY;
//     server_address.sin_port = htons(port);
// }

// void bind_socket() {
//     int bind_status = bind(sock_fd, (const struct sockaddr *) &server_address, sizeof(server_address));
//     if (bind_status < 0) {
//         perror("socket bind to server failed");
//         exit(EXIT_FAILURE);
//     }
// }

// Packet receive_message() {
//     int n;
//     char buffer[buffer_size + 1];
//     n = recvfrom(sock_fd, (char*)buffer, buffer_size, MSG_WAITALL, (struct sockaddr*) &client_address, &len);
//     buffer[n] = '\0';
    
//     return parsePacket(buffer);
// }

// void send_ack(ACK ack) {
//     char str_ack[7];
//     ackToString(ack, str_ack);
//     sendto(sock_fd, str_ack, 6, MSG_CONFIRM, (struct sockaddr*) &client_address, len);
// }
