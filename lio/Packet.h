#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    char ack;
    int nextSequenceNumber;
    char checksum;
} ACK;

typedef struct{
    char soh;
    int sequenceNumber;
    int dataLength;
    char data[1024];
    char checksum;
} Packet;

Packet createPacket(char* data,int sequenceNumber);
void packetToString(Packet p, char* message);
Packet parsePacket(char* packet);
ACK createACK(int nextSequenceNumber);
ACK parseACK(char* ack);
char checksum(char* x, int length);
void ackToString(ACK acknowledgement, char* message);

#endif