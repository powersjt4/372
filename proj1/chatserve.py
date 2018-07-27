from socket import *
import sys

if len(sys.argv) < 2:
	print("Not enough arguments. Exiting.")
	exit();


MESSAGE_BUFFER = 1024
port = int(sys.argv[1])

serverSocket = socket(AF_INET,SOCK_STREAM)
serverSocket.bind(('localhost',port))
serverSocket.listen(1)

while 1:
	print("Waiting for connection...") 
	connectionSocket, addr = serverSocket.accept()
	message = connectionSocket.recv(MESSAGE_BUFFER)
	print("Connection established with " + message +".")
	connection = True 
	while connection:
		message = connectionSocket.recv(MESSAGE_BUFFER)
		if("quit42" in message ):
			connection = False
			break
		print(message)
		print("Host A>"), 
		hostInput = raw_input()
		if (hostInput == "\quit"):
			connectionSocket.send("quit43")
			connectionSocket
			connectionSocket.close()
			break

		connectionSocket.send("HostA> " + hostInput)
