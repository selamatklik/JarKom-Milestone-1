#include "Packet.h"

Packet createPacket(char* data,int sequenceNumber){
    Packet p;
    p.soh=0x1;
    p.sequenceNumber=sequenceNumber;
    int i = 0;
    memset(p.data, '\0', 1025);
    while(*(data+i)!='\0' && i < 1024){
        p.data[i]=*(data+i);
        i++;
    }
    p.data[1024]='\0';
    // printf("\n\n fromcreate: %s\n",p.data);
    p.dataLength=i;
    printf("data length: %d\n", p.dataLength);
    p.checksum=checksum(p.data,p.dataLength);
    return p;
}

void packetToString(Packet p, char* message){
    message[0] = p.soh;
    int i;
    // char * sequenceNumber = (char *) &p.sequenceNumber;
    // message[1] = *sequenceNumber;
    // message[2] = *(sequenceNumber + 1);
    // message[3] = *(sequenceNumber + 2);
    // message[4] = *(sequenceNumber + 3);
    message[1] = p.sequenceNumber & 0xFF;
    message[2] = (p.sequenceNumber >> 8) & 0xFF;
    message[3] = (p.sequenceNumber >> 16) & 0xFF;
    message[4] = (p.sequenceNumber >> 24) & 0xFF;
    // char * dataLength = (char *) &p.dataLength;
    // message[5] = *dataLength;
    // message[6] = *(dataLength + 1);
    // message[7] = *(dataLength + 2);
    // message[8] = *(dataLength + 3);
    message[5] = p.dataLength & 0xFF;
    message[6] = (p.dataLength >> 8) & 0xFF;
    message[7] = (p.dataLength >> 16) & 0xFF;
    message[8] = (p.dataLength >> 24) & 0xFF;
    for (i=0; i<p.dataLength ; i++){
        message[i+9]=p.data[i];
    }
    // message[9+i] = p.data[i];
    message[9+i]=p.checksum;
}

Packet parsePacket(char* packet){
    Packet p;
    int i;
    p.soh=packet[0];
    char tempSeqNumb[4], tempDataNumb[4];
    tempSeqNumb[0] = packet[1];
    tempSeqNumb[1] = packet[2];
    tempSeqNumb[2] = packet[3];
    tempSeqNumb[3] = packet[4];
    tempDataNumb[0] = packet[5];
    tempDataNumb[1] = packet[6];
    tempDataNumb[2] = packet[7];
    tempDataNumb[3] = packet[8];
    p.sequenceNumber = *(int *) tempSeqNumb;
    p.dataLength = *(int *) tempDataNumb;
    // p.sequenceNumber=((int) *(packet+1)) + ((int) *(packet+2)<<8) + ((int) *(packet+3)<<16) + ((int) *(packet+4)<<24);;
    // p.dataLength=((int) *(packet+5)) + ((int) *(packet+6)<<8) + ((int) *(packet+7)<<16) + ((int) *(packet+8)<<24);
    memset(p.data, '\0', 1025);
    for(i=0; i<p.dataLength; i++){
        p.data[i]=packet[i+9];
    }
    // p.data[i+9]='\0';
    p.checksum=packet[9+p.dataLength];
    return p;
}

char checksum(char* x, int length) {
	int sum = 0;
	for (int i =0; i < length; i++) {
		sum += (char) *x;
        *x++;
	}
	return (char) sum;
}

ACK createACK(int nextSequenceNumber){
    ACK acknowledgement;
    acknowledgement.ack='1';
    acknowledgement.nextSequenceNumber=nextSequenceNumber;
    acknowledgement.checksum=checksum((char *)&acknowledgement.nextSequenceNumber,4);
    return acknowledgement;
}

ACK parseACK(char* ack){
    ACK acknowledgement;
    acknowledgement.ack=ack[0];
    acknowledgement.nextSequenceNumber= (int) ack[1];
    acknowledgement.checksum=ack[5];
    return acknowledgement;    
}

void ackToString(ACK acknowledgement, char* message){
    message[0] = acknowledgement.ack;
    char *seq = (char*) &acknowledgement.nextSequenceNumber;
    message[1] = *seq;
    message[2] = *(seq + 1);
    message[3] = *(seq + 2);
    message[4] = *(seq + 3);
    message[5] = acknowledgement.checksum;
}