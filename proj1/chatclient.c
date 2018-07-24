#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


void error(const char*);
void getUsername(char* );

int main(int argc, char *argv[]){
	int socketFD, portNumber;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char message[501];
	char userName[11];
	char buffer[1024];  



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

	getUsername(buffer);
	strcpy(userName, buffer);
	while(1){
		printf("%s> ", userName);
		memset(buffer, 0, 1024);
		fgets(buffer, 1023, stdin);
		send(socketFD, buffer, strlen(buffer), 0);

	}

}

void error(const char *msg) { 
	perror(msg); exit(1); 
} // Error function used for reporting issues


void getUsername(char* rtnName){
	memset(rtnName, 0, 1024);
	printf("Enter user name: ");
	fgets(rtnName, 1023, stdin);
    while(strlen(rtnName) > 10 || strlen(rtnName) <= 0){
    	printf("Enter user name(up to 10 characters): ");
	    fgets(rtnName, 1023, stdin);
    }
    return; 
}
