from socket import *
import sys

def sendMsg(connectionSocket):
	print("Host A>"), 
	hostInput = raw_input()
	if (hostInput == "\quit"):
		connectionSocket.send("quit43")
		connectionSocket.close()
		return False
	connectionSocket.send("HostA> " + hostInput)
	return True

def receiveMsg(connectionSocket):
	message = connectionSocket.recv(MESSAGE_BUFFER)
	if("quit42" in message ):
		connection = False
		return False
	print(message)
	return True
def startUp(connectionSocket):
	message = connectionSocket.recv(MESSAGE_BUFFER)
	print("Connection established with " + message +".")
	return True 

if len(sys.argv) < 2:
	print("Not enough arguments. Exiting.")
	exit();

MESSAGE_BUFFER = 1024
port = int(sys.argv[1])
handle = "Host A> "
serverSocket = socket(AF_INET,SOCK_STREAM)
serverSocket.bind(('',port))
serverSocket.listen(1)

while 1:
	print("Waiting for connection...") 
	connectionSocket, addr = serverSocket.accept()
	connection = startUp(connectionSocket)
	while connection:
		if not receiveMsg(connectionSocket):
			print "Connection closed by client"
			break
		if not sendMsg(connectionSocket):
			break 

