from socket import *
import sys

if len(sys.argv) < 2:
	print "Not enough arguments. Exiting."
	exit();

MESSAGE_BUFFER = 1024
port = int(sys.argv[1])

serverSocket = socket(AF_INET,SOCK_STREAM)
serverSocket.bind(('localhost',port))
serverSocket.listen(1)

print "The server is ready to receive"
while 1:
	connectionSocket, addr = serverSocket.accept()
	message = connectionSocket.recv(MESSAGE_BUFFER)
	print message


