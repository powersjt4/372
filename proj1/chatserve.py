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

print("Waiting for connection...") 
connectionSocket, addr = serverSocket.accept()
message = connectionSocket.recv(MESSAGE_BUFFER)
clientUsername = message+ "> "
print("Connection established.")
while 1:
	sys.stdout.write(clientUsername)
	sys.stdout.flush()
	message = connectionSocket.recv(MESSAGE_BUFFER)
	print(message)
	print("Host A>"), 
	hostInput = raw_input()
	connectionSocket.send(hostInput)

