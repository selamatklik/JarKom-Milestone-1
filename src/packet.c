#include "packet.h"

// merubah packet menjadi string raw
void packet_to_raw(packet pack, char *raw) {
	raw[0] = pack.soh;
	
    char* x = (char*) &pack.seqNum;
	raw[1] = *x;
	raw[2] = *(x+1);
	raw[3] = *(x+2);
	raw[4] = *(x+3);

    char* y = (char*) &pack.dataLength;
    raw[5] = *x;
	raw[6] = *(x+1);
	raw[7] = *(x+2);
	raw[8] = *(x+3);
    
    int i, j = 0;
    for(i=9;i <= 1032; i++){
        raw[i] = pack.data[j];
        j++;
    }
	raw[1033] = pack.checksum;
}

void to_packet(packet* pack, char* raw) {
	pack->soh = *raw;
	pack->seqNum = ((int) *(raw+1)) + ((int) *(raw+2)<<8) + ((int) *(raw+3)<<16) + ((int) *(raw+4)<<24);
	pack->stx =  *(raw+5);
	pack->data = *(raw+6);
	pack->etx = *(raw+7);
	pack->checksum = *(raw+8);
}

void ack_to_raw(packet_ack ack_pack, char* raw) {
	raw[0] = ack_pack.ack;
	char* x = (char*) &ack_pack.nextSeqNum;
	raw[1] = *x;
	raw[2] = *(x+1);
	raw[3] = *(x+2);
	raw[4] = *(x+3);
	raw[5] = ack_pack.windowSize;
	raw[6] = ack_pack.checksum;
}

void to_ack(packet_ack* ack_pack, char* raw) {
	ack_pack->ack = *raw;
	ack_pack->nextSeqNum = ((int) *(raw+1)) + ((int) *(raw+2)<<8) + ((int) *(raw+3)<<16) + ((int) *(raw+4)<<24);
	ack_pack->windowSize = *(raw+5);
	ack_pack->checksum = *(raw+6);	
}

char checksum_str(char* x, int length) {
	int n = 0;
	while(length--) {
		n += (char) *(x++); 
	}
	return (char) n;
}

packet create_packet(int n, int c){
	packet pack;
	pack.soh = 0x1;
	pack.seqNum = n;
	pack.stx = 0x2;
	pack.data = (char)c;
	pack.etx = 0x3;
	pack.checksum = 0x0;
	return pack;
}