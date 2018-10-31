// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT	8085
#define MAXLINE 1024 

int sock_fd, n, len;
struct sockaddr_in server_address;
char buffer[MAXLINE]; 

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
    server_address.sin_port = htons(PORT);
}

void send_message() {
	char msg[20];
	scanf("%s",msg);
	sendto(sock_fd, msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *) &server_address, sizeof(server_address)); 
}

void receive_ack() {
	n = recvfrom(sock_fd, (char *)buffer, MAXLINE, MSG_DONTWAIT, (struct sockaddr *) &server_address, &len); 
	buffer[n] = '\0'; 
	printf("Server : %s\n", buffer); 
}

int main() { 
	memset(&buffer,0,sizeof(buffer));

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
