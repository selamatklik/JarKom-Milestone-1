// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>

int sock_fd, n, len, dest_port, buffer_size;
struct sockaddr_in server_address; 
struct hostent *dest_hostnet;

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
	bcopy(dest_hostnet->h_addr, (char*)&server_address.sin_addr, dest_hostnet->h_length);

    server_address.sin_port = htons(dest_port);
}

void send_message() {
	char msg[20];
	scanf("%s",msg);
	sendto(sock_fd, msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *) &server_address, sizeof(server_address)); 
}

void receive_ack() {
	char* buffer[buffer_size];
	memset(&buffer,0,sizeof(buffer));

	n = recvfrom(sock_fd, (char*) buffer, buffer_size, MSG_DONTWAIT, (struct sockaddr *) &server_address, &len); 
	
	buffer[n] = '\0'; 
	printf("Server : %s\n", buffer); 
}

int main(int argc, char* argv[]) { 
	if (argc == 6) {
		buffer_size = atoi(argv[3]);
		dest_hostnet = gethostbyname(argv[4]);
		dest_port = atoi(argv[5]);
	} else {
		perror("Command format: ./sendfile <filename> <windowsize> <buffersize> <destination_ip> <destination_port>");
		return 1;
	}

	create_socket();
	fill_server_info();
	
	int i=0;
	while (i < 5) {
		send_message();
		receive_ack();
		i++;
	}
	 
	close(sock_fd); 
	return 0; 
} 
