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


// Driver code 
int main() { 
	int sockfd; 
	char buffer[MAXLINE]; 
	char *hello = "Hello from client"; 
	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	
	int n, len; 
	
	int i=0;
	while (i < 10) {
		char msg[20];
		scanf("%s",msg);
		sendto(sockfd, msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 

		n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_DONTWAIT, (struct sockaddr *) &servaddr, 
				&len); 
		buffer[n] = '\0'; 
		printf("Server : %s\n", buffer); 
	
		i++;
	}
	 
	 

	close(sockfd); 
	return 0; 
} 
