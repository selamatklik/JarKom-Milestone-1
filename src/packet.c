#include "packet.h"

// merubah packet menjadi string rawStr
void packet_to_rawStr(packet pack, char *rawStr) {
	rawStr[0] = pack.soh;
	
    char* x = (char*) &pack.seqNum;
	rawStr[1] = *x;
	rawStr[2] = *(x+1);
	rawStr[3] = *(x+2);
	rawStr[4] = *(x+3);

    char* y = (char*) &pack.dataLength;
    rawStr[5] = *x;
	rawStr[6] = *(x+1);
	rawStr[7] = *(x+2);
	rawStr[8] = *(x+3);
    
    int i, j = 0;
    for(i=9;i <= 1032; i++){
        rawStr[i] = pack.data[j];
        j++;
    }
	rawStr[1033] = pack.checksum;
}

void to_packet(packet* pack, char* rawStr) {
	pack->soh = *rawStr;
	pack->seqNum = ((int) *(rawStr+1)) + ((int) *(rawStr+2)<<8) + ((int) *(rawStr+3)<<16) + ((int) *(rawStr+4)<<24);
	pack->dataLength = ((int) *(rawStr+5)) + ((int) *(rawStr+6)<<8) + ((int) *(rawStr+7)<<16) + ((int) *(rawStr+8)<<24);

	int i, j = 0;
    for(i=9;i <= 1032; i++){
        pack.data[j] = *(rawStr+i);
        j++;
    }

	pack->checksum = *(rawStrStr+1033);
}

void ack_to_rawStr(packet_ack ack_pack, char* rawStr) {

	rawStr[0] = ack_pack.ack;
	char* x = (char*) &ack_pack.nextSeqNum;
	rawStr[1] = *x;
	rawStr[2] = *(x+1);
	rawStr[3] = *(x+2);
	rawStr[4] = *(x+3);
	rawStr[5] = ack_pack.checksum;
}

void to_ack(packet_ack* ack_pack, char* rawStr) {
	ack_pack->ack = *rawStr;
	ack_pack->nextSeqNum = ((int) *(rawStr+1)) + ((int) *(rawStr+2)<<8) + ((int) *(rawStr+3)<<16) + ((int) *(rawStr+4)<<24);
	ack_pack->checksum = *(rawStr+5);	
}

char checksum_str(char* x, int length) {
	int n = 0;
	while(length--) {
		n += (char) *(x++); 
	}
	return (char) n;
}

packet create_packet(int n, char * dataStr){
	packet pack;
	pack.soh = 0x1;
	pack.seqNum = n;
	pack.data = dataStr;
	pack.checksum = 0x0;
	return pack;
}