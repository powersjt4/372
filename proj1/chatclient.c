#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


void error(const char*);
void getUsername(char* );
int sendMsg(int, char*);
int receiveMsg(int);

int main(int argc, char *argv[]){
	int socketFD, portNumber;
	int check = 1;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char userName[11];

	if(argc < 2){printf("Not enough arguements. Exit."); exit(0);}

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

	getUsername(userName);
	send(socketFD, userName, strlen(userName), 0);
	
	while(check >= 1){
		check = sendMsg(socketFD, userName);
		check = receiveMsg(socketFD);
	}
}

void error(const char *msg) { 
	perror(msg); exit(1); 
} // Error function used for reporting issues

void getUsername(char* rtnName){
	char buffer[500];
	memset(buffer, 0, 500);
	printf("Enter user name: ");
	fgets(buffer, 500, stdin);
    while(strlen(buffer) > 10 || strlen(buffer) <= 0){
    	printf("Enter user name(up to 10 characters): ");
	    fgets(buffer, 500, stdin);
    }
	strcpy(rtnName,buffer);
	if(rtnName[strlen(rtnName)-1] == '\n')//Strip new line from fgets
		rtnName[strlen(rtnName)-1] = '\0';
    return; 
}

int sendMsg(int socket, char* userName){

		printf("%s> ", userName);
		char buffer[500];
		char sendBuffer[500 + strlen(userName)];
		fgets(buffer, 501, stdin);
		sprintf(sendBuffer,"%s> %s", userName, buffer );
		if(sendBuffer[strlen(sendBuffer)-1] == '\n')//Strip new line from fgets
			sendBuffer[strlen(sendBuffer)-1] = '\0';
		send(socket, sendBuffer, strlen(sendBuffer), 0);
		return 1;
}

int receiveMsg(int socket){
		char buffer[501];
		memset(buffer, 0, 501);
		recv(socket, buffer, 500 , 0);
		printf( "%s\n", buffer);
		return 1;
}
