To compile and run:
	1. From one terminal compile chatserve.py type "python chatserve.py {port number} 
	2. From another terminal type make to compile chatclient.c
	3. Run "./chatclient {hostname} {port number from step 1}" (host name will be "localhost" when client and server are on the same machine)
	4. To close the connection send from either the client or server "\quit"
	5. Sigint {ctrl-c} from the server will close the server


Resources: 
A lot of the C used in the chatclient.c was pulled from my CS344 code which provided a basic amount of code.
https://docs.python.org/2/howto/sockets.html
http://www.bogotobogo.com/python/python_network_programming_tcp_server_client_chat_server_chat_client_select.php
https://www.geeksforgeeks.org/socket-programming-python/