/*
 * Client source file
 * Makes connections with a single Server
 * Server function made with a thread.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>

//Function to create the socket
int socketCreate() {
	int retVal = -1;
	if((retVal = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket not created\n");
	}
	printf("Socket created\n");
	return retVal;
}

//Function to connect the socket
int clientConnect(int aSocket) {
	int retVal = -1;
	int serverPort = 12345;

	//Instantiate socket object and set attributes
	struct sockaddr_in remote = {0};
	remote.sin_family = AF_INET;
	remote.sin_port = htons(serverPort);
	remote.sin_addr.s_addr = inet_addr("127.0.0.1");

	//connect the client server to port and validate
	if(( retVal = connect(aSocket, (struct sockaddr*)&remote, sizeof(remote)) ) < 0) {
		printf("Failed to connect to port\n");
		exit(1);
	}
	printf("Client Connection successful\n");
	return retVal;
}


//Function to send the files to the client
int sender(int aSocket, char* amessage, int messageLen) {
	int retVal = -1;

	/* Udemy course: "Networking" ,C-Intermediate Course,
	 *  Tim Buchalka's Learn Programming Academy
	 */
//	struct timeval tv;
//	tv.tv_sec = 20;  /* 20 Secs Timeout for data send */
//	tv.tv_usec = 0;
//
//	//Set timeout option for Client server Send function
//	if(setsockopt(aSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv)) < 0) {
//		printf("Time Out\n");
//		exit(1);
//	}

	//Send the message
	if (( retVal = send(aSocket, amessage, sizeof(amessage), 0 )) < 0) {
		printf("Message not sent\n");
		exit(1);
	}
	printf("Message Sent\n");
	return retVal;
}

int main(int argc, char* argv[]){
	//ensure the proper number of arguments are passed
	if (argc < 3) {
		perror("Invalid number of arguments\n");
	}

	//instantiate the CL args
	char key[10] = {'\0'};
	strcpy(key, argv[1]);

	char message[200] = {'\0'};
	strcpy(message, argv[2]);
	printf("message is: %s\n", message);

	char serverMessage[255] = {'\0'};

	//Check if the key is > 23
	int intKey = 0;
	sscanf(key, "%d", &intKey);
	if (intKey > 23) {
		printf("invalid key range, re-run Client program\n");
	}

	//Create the client socket
	int sock = socketCreate();

	//Connect the client socket
	clientConnect(sock);

	//Send the encoding key
	sender(sock, key, strlen(key));

	//Send the message to be encoded
	sender(sock, message, strlen(message));

	sleep(3);

	//Close the client Socket
	close(sock);

	return 0;
}

