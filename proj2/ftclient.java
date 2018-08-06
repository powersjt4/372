/** 
 * 
 *  Arguments: 
 */

import java.util.Scanner;
import java.net.*;
import java.io.*;

public class ftclient {

	public static boolean verifyArgs(String[] args) {
		String edu = ".engr.oregonstate.edu";
		System.out.println("edu " + edu + " args[0] " + args[0]);
		if (!("flip1" + edu).equals(args[0]) && !("flip2" + edu).equals(args[0]) && !("flip3" + edu).equals(args[0]) && !"localhost".equals(args[0]) ) {
			System.out.println("Incorrect server host.(flip(1,2,3) or localhost");
			return false;
		}
		try {
			int serverPort = Integer.parseInt(args[1]);
//			int dataPort = Integer.parseInt(args[3]);
		} catch (NumberFormatException | NullPointerException nfe) {
			System.out.println("Incorrect port number format.");
			return false;
		}
/*		
		if (!"-g".equals(args[2]) && !"-l".equals(args[2])) {
			System.out.println("Invalid command only -l (list remote directory or -g(get <filename> from remote directory.");
			return false;
		}
	*/	
		return true;
	}

	public static void main(String []args) {
		Sock pSock = new Sock();

//		if (args.length == 6) {
		if (args.length == 2) {
			if (!verifyArgs(args))
				System.exit(0);
		} else {
			System.out.println("Not enough arguments.");
		}
		while(true){
			pSock.createSocket(Integer.parseInt(args[1]) , args[0]);
			pSock.sendMsg("@");
			pSock.receiveMsg();
		}
	}

}

class Sock {
	Socket socket;
	PrintWriter out;
	BufferedReader in;

	void createSocket(int port, String hostname) {
		//Create socket connection
		try {
			socket = new Socket(hostname, port);
			out = new PrintWriter(socket.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

		} catch (UnknownHostException e) {
			System.out.println("Unknown host: " + hostname);
			System.exit(1);
		} catch  (IOException e) {
			System.out.println("No I/O");
			System.exit(1);
		}
	}

	void sendMsg(String msg) {
		out.println(msg);
	}

	void receiveMsg() {
		System.out.println("Waiting to receive!");
		try {
			in.ready();
			String line = in.readLine();
			System.out.println("Text received: " + line);
		} catch (IOException e) {
			System.out.println("Read failed");
			System.exit(1);
		}
	}
}