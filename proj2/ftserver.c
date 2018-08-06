/*
 * CS 372 Intro. to Computer Networks
 * Project 2
 * Name: Jacob Powers
 * Date: 07/29/18
 * Description: This is the client application for a chat application.
 * The server is started with the chatserve.py with a port number, you can then
 * run this program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

int establishConn(int, char*);
int firstContact(int);
void error(const char*);
void getUsername(char* );
int sendMsg(int) ;
int receiveMsg(int);
int _sendAll(int, char*, int);
void nullTermStr(char*);

int main(int argc, char *argv[]) {
	int check = 1;
	int pSock;
	if (argc == 2) {printf("Not enough arguements. Exit."); exit(0);}
	pSock = establishConn(atoi(argv[2]), argv[1]);
	printf("Connection established initiating handshake...\n");
	firstContact(pSock);

	while (check > 0) {
		check = sendMsg(pSock);
		if (check > 0)
			check = receiveMsg(pSock);
	}
	close(pSock);
	printf("Socket Closed...Goodbye.\n");
	return 0;
}

int establishConn(int portNumber, char* hostName){
	int listenSocketFD, communicationFD; 
	struct sockaddr_in serverAddress, clientAddress;
	socklen_t sizeOfClientInfo;

	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Type of socket
	serverAddress.sin_port = htons(portNumber); // Gets port number from argv
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Gets data from any address

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	printf("Waiting for client connection...\n");
	if (listen(listenSocketFD, 5) < 0) {error("SERVER: Listen socket error");} // Flip the socket on - it can now receive up to 5 connections
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	communicationFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (communicationFD < 0) error("ERROR on accept");
	return communicationFD;

}
/*Collects user name and sends first connection to server*/
int firstContact(int socket) {
	char response[2];	
	char* handshake = "#";

	recv(socket, response, sizeof(response), 0);
	send(socket, handshake, strlen(handshake), 0);
	printf("response = %s\n", response );
	if(strcmp(response, "@\n")==0)
		return -1;
	else
		return 1;
}

// Error function used for reporting issues
void error(const char *msg) {
	perror(msg); exit(1);
}

/* Function: sendMsg
* --------------------
* Gets user input, append handle, call nullTermStr() and send using
* _sendAll(). If user types \quit the connection is closed.
*
* n: socket file descriptor, buffer to send, length of buffer
*
* Returns: 1 on continue or 0 for quit
*/
int sendMsg(int socket) {
	char* quitCode = "quit42";
	char buffer[500];
	printf("send > ");
	fgets(buffer, 501, stdin);
	if (strcmp(buffer, "\\quit\n") == 0) {
		_sendAll(socket, quitCode , strlen(quitCode));
		return 0;
	}
//	sprintf(sendBuffer, "%s> %s", userName, buffer );
//	nullTermStr(buffer);
	_sendAll(socket, buffer, strlen(buffer));
	return 1;
}

/* Function: _sendAll
* --------------------
*  Helper function Sends all data in buf
*
* n: socket file descriptor, buffer to send, length of buffer
*
* Returns: number of characters sent
*/
int _sendAll(int s, char *buf, int len)
{
	int bytesleft = len;
	int sent = 0;
	while (sent < len) {
		//if (TEST) {printf("%d bytes sent of %d.\n", sent, len);}
		int	n = send(s, buf + sent, bytesleft, 0);
		if (n == -1) {error("Failed to send data"); return -1;}
		sent += n;
		bytesleft -= n;
	}
	return sent;
}
/* Function: receiveMsg
* --------------------
* Receives a message from the server and prints to console.
*
* n: the socket number
*
* Returns: Returns 1 on continue and returns 0 for exit and close
*/
int receiveMsg(int socket) {
	char buffer[501];
	memset(buffer, 0, 501);
	recv(socket, buffer, 500 , 0);// Receive size of next message from server
	if (strcmp(buffer, "quit43") == 0) {
		return 0;
	}
	printf( "%s\n", buffer);
	return 1;
}
/* Function: nullTermStr
* --------------------
* The string to remove the \n.
*
* n: A string passed as pointer
*
* Returns: void but null terminated string is passed
*/
void nullTermStr(char* str) {
	if (str[strlen(str) - 1] == '\n') //Strip new line from fgets
		str[strlen(str) - 1] = '\0';
}

