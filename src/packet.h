#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdio.h>
#include <stdlib.h>
#include "segment.h"

typedef struct {
	char soh;
	int seqNum;
	int dataLength;
	char data[1024];
	char checksum;
} packet;

typedef struct {
	char ack;
	int nextSeqNum;
	char checksum;
} packet_ack;

void packet_to_rawStr(packet pack, char* rawStr);

void to_packet(packet* pack, char* rawStr);

void ack_to_rawStr(packet_ack ack_pack, char* rawStr);

void to_ack(packet_ack* ack_pack, char* rawStr);

char checksum_str(char* x, int len);

packet create_packet(int n, char * dataStr);

#endif