#include "Packet.h"

Packet createPacket(char* data,int sequenceNumber){
    Packet p;
    p.soh=0x1;
    p.sequenceNumber=sequenceNumber;
    int i = 0;
    while(*(data+i)!='\0' && i < 1024){
        p.data[i]=*(data+i);
        i++;
    }
    p.dataLength=i;
    p.checksum=checksum(p.data,p.dataLength);
    return p;
}

void packetToString(Packet p, char* message){
    message[0] = p.soh;
    int i;
    char * sequenceNumber = (char *) &p.sequenceNumber;
    message[1] = *sequenceNumber;
    message[2] = *(sequenceNumber + 1);
    message[3] = *(sequenceNumber + 2);
    message[4] = *(sequenceNumber + 3);
    char * dataLength = (char *) &p.dataLength;
    message[5] = *dataLength;
    message[6] = *(dataLength + 1);
    message[7] = *(dataLength + 2);
    message[8] = *(dataLength + 3);
    for (i=0; i<p.dataLength ; i++){
        message[i+9]=p.data[i];
    }
    message[10+i]=p.checksum;
}

Packet parsePacket(char* packet){
    Packet p;
    int i;
    p.soh=packet[0];
    p.sequenceNumber=(int) packet[1];
    p.dataLength=(int) packet[5];
    for(i=0; i<p.dataLength; i++){
        p.data[i]=packet[i+9];
    }
    p.checksum=packet[10+i];
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