/*
 * CS 372 Intro. to Computer Networks
 * Project 1
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
void firstContact(int, char*);
void error(const char*);
void getUsername(char* );
int sendMsg(int, char*);
int receiveMsg(int);
int _sendAll(int, char*, int);
void nullTermStr(char*);

int main(int argc, char *argv[]) {
	int check = 1;
	char userName[11];
	int pSock;
	if (argc == 2) {printf("Not enough arguements. Exit."); exit(0);}
	pSock = establishConn(atoi(argv[2]), argv[1]);
	firstContact(pSock);

	while (check > 0) {
		check = sendMsg(pSock, userName);
		if (check > 0)
			check = receiveMsg(pSock);
	}
	close(pSock);
	printf("Socket Closed...Goodbye.\n");
	return 0;
}

int establishConn(int portNumber, char* hostName){
	int socketFD; 
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverHostInfo = gethostbyname(hostName);
	if (serverHostInfo == NULL) { fprintf(stderr, "Server: ERROR, no such host\n"); exit(0); }
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("Server: ERROR opening socket");

	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("Server: ERROR connecting");
	
	return socketFD;

}
/*Collects user name and sends first connection to server*/
void firstContact(int socket, char* userName) {
	char response;	
	char handshake = '#';	
	send(socket, handshake, strlen(userName), 0);
	recv(socket, response, 1, 0);// Receive size of next message from server
	if(response != @)
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
int sendMsg(int socket, char* userName) {
	char* quitCode = "quit42";
	printf("%s> ", userName);
	char buffer[500];
	char sendBuffer[500 + strlen(userName)];
	fgets(buffer, 501, stdin);
	if (strcmp(buffer, "\\quit\n") == 0) {
		_sendAll(socket, quitCode , strlen(quitCode));
		return 0;
	}
	sprintf(sendBuffer, "%s> %s", userName, buffer );
	nullTermStr(sendBuffer);
	_sendAll(socket, sendBuffer, strlen(sendBuffer));
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

