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
#include <dirent.h>
#include <ctype.h>

struct cmd
{  char command[2];
   int dataPort;
   char filename[50];
};

void sendList(int);
int sendFile(int, int, char*);
int processCommand(int,int, struct cmd*);
int receiveCommands(int, struct cmd*);
int establishConn(int);
int handshake(int, char*);
void error(const char*);
void getUsername(char* );
int sendMsg(int) ;
int receiveMsg(int);
int _sendAll(int, char*, int);
void nullTermStr(char*);

int main(int argc, char *argv[]) {
	int check = 0;
	int pSock, qSock;
	//char buffer[500];
	char clientHostname[500];
	struct cmd commands;
	if (argc == 1) {printf("Not enough arguements. Exit."); exit(0);}

	pSock = establishConn(atoi(argv[1]));
	printf("(%d)Server open on %s...\n", check,argv[1] );
	check = handshake(pSock,clientHostname);
	printf("(%d)Connection from %s...\n", check, clientHostname);
	check = receiveCommands(pSock, &commands);
	printf("(%d)Command valid processing command (%s)...\n", check,commands.command);
	qSock = establishConn(commands.dataPort); 
	processCommand(pSock, qSock, &commands);
	close(pSock);
	close(qSock);
	printf("Socket Closed...Goodbye.\n");
	return 0;
}

int receiveCommands(int pSock, struct cmd* rtncmds){
	char buffer[500];
	int numArgs = 0;
	memset(buffer, 0, 500);
	recv(pSock, buffer, sizeof(buffer), 0);// Receive command
	nullTermStr(buffer);
	numArgs = buffer[0] -'0';
	printf("response2 = (%s) and numArgs = %c\n", buffer, buffer[0]);
	if(numArgs == 2){
		sscanf(buffer,"%d %s %d", &numArgs, rtncmds->command, &rtncmds->dataPort);
	} else if(numArgs == 3){
		sscanf(buffer,"%d %s %s %d", &numArgs, rtncmds->command,rtncmds->filename, &rtncmds->dataPort);
	}	
	//printf("Command = %s -- dataPort = %d filename = %s\n", rtncmds->command, rtncmds->dataPort, rtncmds->filename );
	return 0;
}

int processCommand(int pSock, int qSock, struct cmd* commands){
	char* ok ="ok\n";
	char* error = "Invalid Command\n";
	char* fileError = "File not found\n";
	if (strcmp(commands->command, "-l")== 0) { // client sent -l send directory list
		_sendAll(pSock, ok, strlen(ok));//Sends ok to client to initiate listening on qSock
		sendList(qSock);
		return 1;
	} else if (strcmp(commands->command, "-g")== 0){ //Client sent -g send file
		_sendAll(pSock, ok, strlen(ok));//Sends ok to client to initiate listening on qSock
		if(sendFile(pSock, qSock, commands->filename) > 0){
			return 2;
		} else {
			printf("%s\n", fileError);  //Client sent an invalid command
			_sendAll(pSock, fileError, strlen(fileError));
			return -1;
		}
	}else{
		printf("%s\n", error);  //Client sent an invalid command
		_sendAll(pSock, error, strlen(error));
		return -1;
	}

}

int sendFile(int pSock, int qSock, char* filename){
	FILE* fp;
	int filelines = 0;
	char buffer[500];
	char ch;
	memset(buffer, 0, 500);
	if (access(filename, F_OK) == -1){  // Counter number of words in file
		return -1;
	}	
	fp = fopen(filename, "r");
	while((ch = getc(fp)) != EOF){
		fscanf(fp,"%s",buffer);
		if(ch=='\n') //Counts number of lines in file
			filelines++;
	}
	sprintf(buffer,"%d\n", filelines);
	_sendAll(pSock, buffer, sizeof(buffer)); //Send number of words in file
	rewind(fp);
	printf("Lines left = %d ",filelines);
	memset(buffer, 0, 500);
	
	while(fgets(buffer,sizeof(buffer),fp)){
		_sendAll(qSock, buffer, sizeof(buffer));
		printf("Lines left = %d ",--filelines);
		printf("%s", buffer);
	}
	printf("File Sent to client\n");
	fclose(fp);
	return 0;
}

void sendList(int qSock){
	struct dirent *entry;;
	char buffer[500];
	DIR *curDir = opendir(".");
	if (curDir == NULL) {
		printf("Could not get directory contents.");
		return;
	}

						// Error is sent in processCommand function.
	while ((entry = readdir(curDir)) != NULL) {
		printf("%s\n", entry->d_name);
		sprintf(buffer, "%s\n", entry->d_name);
		_sendAll(qSock, buffer, strlen(buffer));
	}
	_sendAll(qSock, "%%", 2);
	closedir(curDir);
}

/*Collects user name and sends first connection to server*/
int handshake(int socket,char* clientHostname) {
	char response[500];
	char* handshake = "#\n";
	recv(socket, response, sizeof(response), 0);
//	printf("Response 1 = (%s)", response);// Should be hostname
	nullTermStr(response);
	strcpy(clientHostname, response);
	send(socket, handshake, strlen(handshake), 0);
	memset(response, 0, 500);
	recv(socket, response, sizeof(response), 0);
//	printf("Response 2 = (%s)", response ); // should be @
	if (strcmp(response, "@\n") != 0)
		return -1;
	return 0;
}

int establishConn(int portNumber) {
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


