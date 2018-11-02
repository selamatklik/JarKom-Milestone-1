#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include "Packet.h"

int sock_fd, len, window_size, buffer_size, port;
struct sockaddr_in server_address, client_address;

void create_socket() {
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
}

void fill_server_info() {
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
}

void bind_socket() {
    int bind_status = bind(sock_fd, (const struct sockaddr *) &server_address, sizeof(server_address));
    if (bind_status < 0) {
        perror("socket bind to server failed");
        exit(EXIT_FAILURE);
    }
}

Packet receive_message() {
    int n;
    char buffer[buffer_size + 1];
    n = recvfrom(sock_fd, (char*)buffer, buffer_size, MSG_WAITALL, (struct sockaddr*) &client_address, &len);
    buffer[n] = '\0';
    
    return parsePacket(buffer);
}

void send_ack(ACK ack) {
    char str_ack[7];
    ackToString(ack, str_ack);
    sendto(sock_fd, str_ack, 6, MSG_CONFIRM, (struct sockaddr*) &client_address, len);
}

int main(int argc, char* argv[]) {

    if (argc == 5) {
        window_size = atoi(argv[2]);
        buffer_size = 1024 * atoi(argv[3]);
        port = atoi(argv[4]);
    } else {
        perror("Command format: ./recvfile <filename> <windowsize> <buffersize> <port>");
        return 1;
    }

    create_socket();
    fill_server_info();
    bind_socket();

    memset(&client_address,0,sizeof(client_address));

    int finish = 0;
    while (!finish) {
        int arr_frame_rcv[window_size];
        for (int i=0; i<window_size; i++) arr_frame_rcv[i] = 0;

        int last_frame_rcv = -1;
        int largest_acc_frame = last_frame_rcv + window_size;

        Packet curr_packet = receive_message();
        printf("%s\n",curr_packet.data);
        ACK curr_ack = createACK(curr_packet.sequenceNumber + 1);
        send_ack(curr_ack);
        // if (curr_packet.checksum == checksum(curr_packet.data, curr_packet.dataLength)) {
        //     ACK curr_ack = createACK(curr_packet.sequenceNumber + 1);
        //     send_ack(curr_ack);
        // }
        
    }

    close(sock_fd);
    return 0;
}