To compile and run:
	1. In a terminal type make. This will compplie the ftserver and ftflient 
	2. To test start up the server by typing "./ftserver <Server_Port_Number>"
	3. Next start the client with the list command (-l)"java ftclient <Server_Hostname flip1,2,3 or localhost> <Server_Port_Number> <Command -l> <Data_Port_Number>"
	   -or- start up with the get command (-g) "java ftclient <Server_Hostname flip1,2,3 or localhost> <Server_Port_Number> <Command -g> <Filename_To_Get><Data_Port_Number>"
	4. Sigint {ctrl-c} from the server will close the server

	If the file already exists at the client location copy will be appended.

Resources: 
A lot of the C used in the chatclient.c was pulled from my CS344 code which provided a basic amount of code.
http://www.oracle.com/webfolder/technetwork/tutorials/obe/java/SocketProgramming/SocketProgram.html
https://www.oracle.com/technetwork/java/socket-140484.html#client
http://profesores.elo.utfsm.cl/~agv/elo329/Java/javamakefile.html
https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
https://crunchify.com/how-to-get-server-ip-address-and-hostname-in-java/
https://www.youtube.com/watch?v=9g_nMNJhRVk ->Transferring a text file in Socket Programming in TCP
https://www.mkyong.com/java/how-to-write-to-file-in-java-bufferedwriter-example/