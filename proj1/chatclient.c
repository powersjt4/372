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

void firstContact(int, char*);
void error(const char*);
void getUsername(char* );
int sendMsg(int, char*);
int receiveMsg(int);
int _sendAll(int, char*, int);
void nullTermStr(char*);

int main(int argc, char *argv[]) {
	int socketFD, portNumber;
	int check = 1;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char userName[11];

	if (argc < 2) {printf("Not enough arguements. Exit."); exit(0);}

	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverHostInfo = gethostbyname(argv[1]);
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	firstContact(socketFD, userName);

	while (check > 0) {
		check = sendMsg(socketFD, userName);
		if (check > 0)
			check = receiveMsg(socketFD);
	}
	close(socketFD);
	printf("Socket Closed...Goodbye.\n");
	return 0;
}

/*Collects user name and sends first connection to server*/
void firstContact(int socket, char* userName) {
	getUsername(userName);
	send(socket, userName, strlen(userName), 0);
	return;
}

// Error function used for reporting issues
void error(const char *msg) {
	perror(msg); exit(1);
}
/* Function: getUsername
* --------------------
* Gets user name from user, check for <10 characters, strip \n
*
* n: return pointer for user name string
*
* Returns: String passed as reference
*/
void getUsername(char* rtnName) {
	char buffer[500];
	memset(buffer, 0, 500);
	printf("Enter user name: ");
	fgets(buffer, 500, stdin);
	while (strlen(buffer) > 10 || strlen(buffer) <= 0) {
		printf("Enter user name(up to 10 characters): ");
		fgets(buffer, 500, stdin);
	}
	strcpy(rtnName, buffer);
	nullTermStr(rtnName);
	return;
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
