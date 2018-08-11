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
{  char hostname[256];
	char command[2];
   int dataPort;
   char filename[50];
};

void sendList(int);
int sendFile(int, int, char*);
int processCommand(int,int, struct cmd*);
int receiveCommands(int, struct cmd*);
int establishConn(int);
int establishConnNew(int, int);
int handshake(int, struct cmd*);
void error(const char*);
void getUsername(char* );
int _sendAll(int, char*, int);
void nullTermStr(char*);

int main(int argc, char *argv[]) {
	int check = 0;
	int pSock, qSock, comSock, dataSock;
	struct cmd clientInfo;
	if (argc == 1) {printf("Not enough arguements. Exit."); exit(0);}
	comSock = establishConnNew(atoi(argv[1]), 0);
//	printf("Server open on %s....\n", argv[1]);
	while(1){
			struct sockaddr_in serverAddress;
			socklen_t sizeOfClientInfo;
			pSock = accept(comSock, (struct sockaddr *)&serverAddress, &sizeOfClientInfo);
			if(pSock < 0){
				check = -1;
			error("Server Error Accepting Connection.\n");
			}
			check = handshake(pSock, &clientInfo);
			printf("Connection from %s...\n", clientInfo.hostname);
			check = receiveCommands(pSock, &clientInfo);
//			printf("(%d)Command valid processing command (%s)...\n", check,clientInfo.command);
			dataSock = establishConnNew(clientInfo.dataPort, pSock); 
			qSock = accept(dataSock, NULL, NULL);
			if(pSock < 0){
				check = -1;
				error("Server Error Accepting Connection.\n");
			}
//			printf("qSocket created processing commands...\n" );
			processCommand(pSock, qSock, &clientInfo);
			close(qSock);
			close(dataSock);
			close(pSock);
		
	}
	close(comSock);
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
//	printf("response2 = (%s) and numArgs = %c\n", buffer, buffer[0]);
	if(numArgs == 2){
		sscanf(buffer,"%d %s %d", &numArgs, rtncmds->command, &rtncmds->dataPort);
	} else if(numArgs == 3){
		sscanf(buffer,"%d %s %s %d", &numArgs, rtncmds->command,rtncmds->filename, &rtncmds->dataPort);
	}	
	//printf("Command = %s -- dataPort = %d filename = %s\n", rtncmds->command, rtncmds->dataPort, rtncmds->filename );
	return 0;
}

int processCommand(int pSock, int qSock, struct cmd* commands){
	char* ack  ="ack\n";
	char* error = "Invalid Command\n";
	char* fileError = "File not found\n";
	if (strcmp(commands->command, "-l")== 0) { // client sent -l send directory list
		printf("List directory requested on port...");
		_sendAll(pSock, ack, strlen(ack));//Sends ok to client to initiate listening on qSock
		sendList(qSock);
		return 1;
	} else if (strcmp(commands->command, "-g")== 0){ //Client sent -g send file
		_sendAll(pSock, ack, strlen(ack));//Sends ok to client to initiate listening on qSock
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
	memset(buffer, 0, 500);
	if (access(filename, F_OK) == -1){  // Counter number of words in file
		return -1;
	}	
	fp = fopen(filename, "r");
	while(fgets(buffer,sizeof(buffer),fp)){
			filelines++;
	}
	sprintf(buffer,"%d\n", filelines);
	_sendAll(pSock, buffer, sizeof(buffer)); //Send number of words in file
	rewind(fp);
//	printf("Lines left = %d ",filelines);
	
	printf("port...");
	memset(buffer, 0, 500);
	while(fgets(buffer,sizeof(buffer),fp)){
		_sendAll(qSock, buffer, sizeof(buffer));
		printf("Lines left = %d ",--filelines);
		printf("%s", buffer);

		memset(buffer, 0, 500);
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
int handshake(int socket,struct cmd* clientInfo) {
	char response[500];
	char* handshake = "#\n";
	recv(socket, response, sizeof(response), 0);
	nullTermStr(response);
	strcpy(clientInfo->hostname, response);
	send(socket, handshake, strlen(handshake), 0);
	memset(response, 0, 500);
	recv(socket, response, sizeof(response), 0);
	if (strcmp(response, "@\n") != 0)
		return -1;
	return 0;
}

int establishConnNew(int portNumber, int previousSock){
	int listenSocketFD;
	struct sockaddr_in serverAddress;
	char* handshake = "@@\n";
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Type of socket
	serverAddress.sin_port = htons(portNumber); // Gets port number from argv
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Gets data from any address
		// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

		// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to port
		error("ERROR on binding");
	}
	if(previousSock > 0){
		_sendAll(previousSock, handshake, strlen(handshake)); 
	}
	printf("Listening on port %d\n", portNumber);	
	if (listen(listenSocketFD, 5) < 0) {error("SERVER: Listen socket error");} // Flip the socket on - it can now receive up to 5 connections

	return listenSocketFD;
}

// Error function used for reporting issues
void error(const char *msg) {
	perror(msg); exit(1);
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


