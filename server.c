/*
 * Server source file
 * Allows connections with '3' clients as defined in the listen function
 * Performs Caesar Cypher operation on the message sent by the client.
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

/* Create the Socket */
/*
 * Function to create the Socket
 * P: None
 * R: An integer to validate the Socket's creation
 */
int sockCreate() {
	int mySock = -1;	//Instantiated as function failed
	if ((mySock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket not created\n");
	}
	printf("Socket created\n");
	return mySock;
}

/* Set the Socket Options */
/*
 * Function to Set the socket options
 * P: The mySock, return int of sockCreate() call
 * R: An integer to validate parameters have been set
 */
int setSocketOpt(int aSock) {
	int optRet = -1, enable = 1;
	//Set the socket opt
	if((optRet = setsockopt(aSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) < 0) {
		perror("Failed to Set socket options...\n");
		exit(1);
	}
	printf("Successfully set socket options...\n");
	return optRet;
}
/* Bind the Socket */
/*
 * Function to bind the socket to a port
 * P: The mySock, return int of sockCreate() call
 * R: An integer to validate the Socket's binding
 */
int sockBind(int aSock) {
	int bindRetVal = -1;
	int clientPort = 12345;
	//instantiate IP Socket Address
	struct sockaddr_in remote;
	//Set the struct variables
	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = htonl(INADDR_ANY);
	remote.sin_port = htons(clientPort);

	if((bindRetVal = bind(aSock, (struct sockaddr*)&remote, sizeof(remote))) < 0) {
		perror("Bind Unsuccessful...\n");
		exit(1);
	}
	printf("Bind Successful\n");
	return bindRetVal;
}
/* Listen for client conections */
/*
 * Function to listen for client connections
 * P: The mySock, return int of sockCreate() call
 * R: An integer to validate the function's successful call
 */
int svrListen(int aSock) {
	int lstnRetVal = -1;
	if ((lstnRetVal = listen(aSock, 3)) < 0) {
		perror("Listen call failed\n");
		exit(1);
	}
	printf("Listen successful\n");
	return lstnRetVal;
}

/* Accept a connection from the client*/
/*
 * Function that accepts connections from connecting client servers
 * P: Socket descriptor from function call
 * 					&
 * 	struct sockaddr objec pointer
 * R: Returns zero on success.
 */
int acptConnect(int aSock, struct sockaddr_in cliAddr) {
	int acptRetVal = -1;
	int clientLen = sizeof(cliAddr);
	if ((acptRetVal = accept(aSock, (struct sockaddr*)&cliAddr, (socklen_t*)&clientLen)) < 0) {
		perror("accept call failed\n");
		exit(1);
	}
	printf("Accept call successful\n");
	return acptRetVal;
}

/* Pointer to the server function to enable threading */
/*
 * Function to allow a server to be created using a thread
 * P: The message to be sent to the client, and buffer to receive a message from the client
 * R: it returns the message received from the client
 */
void *server () {
	char encdKey[2];
	char clientMessage[20];				//Char array to receive the clients message
	char* serverMessage = "Hello from Server";	//server message
	int sockDesc = 0;							//Storing socket() return val
	int acptVal = 0;							//Storing the accept return val

	//Instantiate socket for client server
	struct sockaddr_in client;

	//Create the socket
	sockDesc = sockCreate();

	//set Socket opts
	setSocketOpt(sockDesc);

	//Bind the socket
	sockBind(sockDesc);

	//Continuously listen for a new client connection
	while(1) {
		//Listen for connections (embedded in a while loop)
		svrListen(sockDesc);
		printf("\nWaiting for incoming client connections\n");

		//Accept connections
		acptVal = acptConnect(sockDesc, client);

		printf("%s\n", serverMessage);

		/*Receive the clients message (includes an encoding key and message)*/
		////Set up the buffers to receive the message from the client
		memset(encdKey, '\0', sizeof(encdKey));
		memset(clientMessage, '\0', sizeof(clientMessage));

		//Receive Key
		if (recv(acptVal, encdKey, 10, 0) < 0){
			printf("Encoding Key Receive failed\n");
			break;
		}
		//store the encoding key
		int key = 0;
		sscanf(encdKey, "%d", &key);
		printf("Encoding key is: %d\n", key);
		sleep(2);

		//Receive Message
		if (recv(acptVal, clientMessage, 200, 0) < 0) {
			printf("Message Receive failed\n");
			break;
		}
		printf("Message to encode is: %s\n", clientMessage);

		//Use a Caesar Cypher on the message
		/* Encrypytion process */
		for(int i = 0; i < sizeof(clientMessage); i++) {
			char let = clientMessage[i];
			//lower case letters
			if (let >= 'a' && let <= 'z') {
				let = let + key;
				if (let > 'z') {
					let = let - 'z' + 'a' - 1;
				}
				clientMessage[i]  = let;
			}
			//Upper case letters
			else if (let >= 'A' && let <= 'Z') {
				let = let + key;
				if (let > 'Z') {
					let = let - 'Z' + 'A'- 1;
				}
				clientMessage[i] = let;
			}
		}

		printf("The Encoded message is: %s\n", clientMessage);


		close(acptVal);
		sleep(1);
	}
}

int main() {
	pthread_t athread;		//Call a pthread instance
	printf("\nCalling a thread to call server function...\n\n");

	//Create a thread to call the Server function
	pthread_create(&athread, NULL, server, NULL);

	//wait for thread to return
	pthread_join(athread, NULL);

	//Delete the thread instance
	pthread_exit(NULL);

	return 0;
}
