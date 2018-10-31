#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>

#define PORT 8085
#define MAXLINE 1024

int sock_fd, len;
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
    server_address.sin_port = htons(PORT);
}

void bind_socket() {
    int bind_status = bind(sock_fd, (const struct sockaddr *) &server_address, sizeof(server_address));
    if (bind_status < 0) {
        perror("socket bind to server failed");
        exit(EXIT_FAILURE);
    }
}

void receive_message() {
    int n;
    char buffer[MAXLINE];
    n = recvfrom(sock_fd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &client_address, &len);
    buffer[n] = '\0';

    printf("Client: %s\n", buffer);
}

void send_ack() {
    char* ack = "ACK";
    sendto(sock_fd, ack, strlen(ack), MSG_CONFIRM, (struct sockaddr*) &client_address, len);
}

int main() {
    create_socket();
    fill_server_info();
    bind_socket();

    memset(&client_address,0,sizeof(client_address));

    int i=0;
    while (i <5) {
        receive_message();
        send_ack();
        i++;
    }

    close(sock_fd);
    return 0;
}