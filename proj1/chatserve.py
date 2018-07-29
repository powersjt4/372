
# CS 372 Intro. to Computer Networks
# Project 1
# Name: Jacob Powers
# Date: 07/29/18
# Description: This is the server code for a chat application.
# The client is started with the "./chatclient {hostname} {port number}", you must
# run this server first by running "python chatserve.py {port number}.
 

from socket import *
import sys

#Gets and sends message to client
#\quit will cloase connection to client
def sendMsg(connectionSocket):
	print("Host A>"), 
	hostInput = raw_input()
	if (hostInput == "\quit"):
		connectionSocket.send("quit43")
		connectionSocket.close()
		return False
	connectionSocket.send("HostA> " + hostInput)
	return True

#Receive message from client return false if client
#sends \quit this will break loop in second while loop
def receiveMsg(connectionSocket):
	message = connectionSocket.recv(MESSAGE_BUFFER)
	if("quit42" in message ):
		connection = False
		return False
	print(message)
	return True

#Starts the connection with the client
def startUp(connectionSocket):
	message = connectionSocket.recv(MESSAGE_BUFFER)
	print("Connection established with " + message +".")
	return True 

if len(sys.argv) < 2:
	print("Not enough arguments. Exiting.")
	exit();

MESSAGE_BUFFER = 500
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

