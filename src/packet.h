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

void packet_to_raw(packet pack, char* raw);

void to_packet(packet* pack, char* raw);

void ack_to_raw(packet_ack ack_pack, char* raw);

void to_ack(packet_ack* ack_pack, char* raw);

char checksum_str(char* x, int len);

packet create_packet(int n, int data);

#endif