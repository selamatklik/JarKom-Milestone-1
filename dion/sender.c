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
#include <sys/time.h>
#include <pthread.h>

#define BUFFER_LEN 512
#define TIME_OUT 5000	// milliseconds

// Socket variable
int sock_fd, n, len, dest_port;
struct sockaddr_in server_address; 
struct hostent *dest_hostnet;

// sliding window variable
int buffer_size, window_size, last_ack_rcv, last_frame_send;

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

int get_ack_index() {
	return 0;
}

void* receive_ack(int* arr_ack_state, unsigned long* arr_frame_time) {

	char* buffer[buffer_size];
	memset(&buffer,0,sizeof(buffer));

	int ack_len;
	while(1) {
		ack_len = recvfrom(sock_fd, (char*) buffer, buffer_size, MSG_WAITALL, (struct sockaddr *) &server_address, &len); 
		
		int ack_index = get_ack_index();
		int error_ack = 1;
		if (!error_ack) {
			if (last_ack_rcv < ack_index && ack_index <= last_frame_send) {
				arr_ack_state[ack_index - (last_ack_rcv + 1 )] = 1;
			} else {
				struct timeval now;
				gettimeofday(&now, NULL);
				arr_frame_time[ack_index - (last_ack_rcv + 1)] = now.tv_usec;
			}
		}
	}
}

// void receive_ack() {
// 	char* buffer[buffer_size];
// 	memset(&buffer,0,sizeof(buffer));

// 	n = recvfrom(sock_fd, (char*) buffer, buffer_size, MSG_DONTWAIT, (struct sockaddr *) &server_address, &len); 
	
// 	buffer[n] = '\0'; 
// 	printf("Server : %s\n", buffer); 
// }

void read_file(char* message, char *file_name) {
	if (access(file_name, F_OK) == -1) {
		printf("File not found\n");
		exit(EXIT_FAILURE);
	} else {
		FILE *file = fopen(file_name,"r");
		char c = fgetc(file); int i=0;
		while (c != EOF && i < BUFFER_LEN) {
			message[i] = c;
			i++; c = fgetc(file);
		}

		printf("Read file completed\n");
		fclose(file);
	}
}

int step_to_slide(int* arr_ack_state) {
	if (arr_ack_state[0]) {
		int nb_step = 1;
		for (int i=0; i<window_size; i++) {
			if (!arr_ack_state[i]) {
				break;
			}
			nb_step++;
		}
		return nb_step;
	}
	return 0;
}

void slide_window(int step, int* arr_ack_state, int* arr_frame_send, int* arr_frame_time) {
	if (step == 0) return;

	for (int i=0; i<window_size-step; i++) {
		arr_ack_state[i] = arr_ack_state[i+step];
		arr_frame_send[i] = arr_frame_send[i+step];
		arr_frame_time[i] = arr_frame_time[i+step];
	}
	for (int i=window_size-step; i<window_size; i++) {
		arr_frame_send[i] = 0;
		arr_ack_state[i] = 0;
	}
	last_ack_rcv += step;
	last_frame_send = last_ack_rcv + window_size;
}

int main(int argc, char* argv[]) { 
	char *file_name;
	char message[BUFFER_LEN];

	if (argc == 6) {
		file_name = argv[1];
		window_size = atoi(argv[2]);
		buffer_size = atoi(argv[3]);
		dest_hostnet = gethostbyname(argv[4]);
		dest_port = atoi(argv[5]);
	} else {
		perror("Command format: ./sendfile <filename> <windowsize> <buffersize> <destination_ip> <destination_port>");
		exit(EXIT_FAILURE);
	}
	
	read_file(message,file_name);
	
	create_socket();
	fill_server_info();
	
	int arr_ack_state[window_size], arr_frame_send[window_size];
	unsigned long arr_frame_time[window_size];

	memset(arr_ack_state, 0, sizeof(arr_ack_state));
	memset(arr_frame_send, 0, sizeof(arr_frame_send));
	for (int i=0; i<window_size; i++) {
		struct timeval start;
		gettimeofday(&start, NULL);
		arr_frame_time[i] = start.tv_usec;
	}
	
	int nb_frame = 10;
	int frame_number;

	last_ack_rcv = -1;
	last_frame_send = last_ack_rcv + window_size;

	pthread_t rcv_ack_thread;
	int rcv_ack_thread_state;

	rcv_ack_thread_state = pthread_create(&rcv_ack_thread,NULL,&receive_ack,NULL);

	int finish = 0;
	while (!finish) {
		int nb_step = step_to_slide(arr_ack_state);
		slide_window(nb_step,arr_ack_state,arr_frame_send,arr_frame_time);
		
		for (int i=0; i<window_size; i++) {
			frame_number = last_ack_rcv + i + 1;
			if (frame_number < nb_frame) {
				
				struct timeval now;
				gettimeofday(&now, NULL);
				unsigned long diff_time = now.tv_usec - arr_frame_time[i];
				int loss = !arr_ack_state[i] && diff_time > TIME_OUT;

				if (!arr_frame_send[i] || loss) {
					send_message();
					arr_frame_send[i] = 1;
					arr_frame_time[i] = now.tv_usec;
				}
			}
		}
		finish = 1;
	}

	close(sock_fd); 
	return 0; 
} 
