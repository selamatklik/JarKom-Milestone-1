all: sendfile recvfile

sendfile: UDPClient.c Utility.c Packet.c
	gcc UDPClient.c Utility.c Packet.c -o sendfile -lwsock32

recvfile: UDPClient.c Utility.c Packet.c
	gcc UDPServer.c Utility.c Packet.c -o recvfile -lwsock32

clear:
	rm *o sendfile recvfile