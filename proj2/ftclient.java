/**
 *
 */
import java.util.Scanner;
import java.net.*;
import java.io.*;


public class ftclient {
	/**
		args[0] = server host , args[1] = server port number, args[2] = commands -l or -g,
		args[3] = data port for -l or filename for -g, args[4] = data port for -g
	**/

	public static boolean verifyArgs(String[] args) {
		if (!("flip1").equals(args[0]) && !("flip2").equals(args[0]) && !("flip3").equals(args[0]) && !"localhost".equals(args[0]) ) {
			System.out.println("Incorrect server host.(flip(1,2,3) or localhost");
			return false;
		}
		try {
			int serverPort = Integer.parseInt(args[1]);
			if (args.length == 5) {
				int dataPort = Integer.parseInt(args[4]); //Command is -g uses 5 args
			} else if (args.length == 4) {
				int dataPort = Integer.parseInt(args[3]); // Commands i -l uses 4 args
			}
		} catch (NumberFormatException | NullPointerException nfe) {
			System.out.println("Incorrect port number format.");
			return false;
		}
		if (!"-g".equals(args[2]) && !"-l".equals(args[2])) {
			System.out.println("Invalid command only -l (list remote directory) or -g(get <filename> from remote directory).");
			return false;
		}

		return true;
	}
	public static boolean handshake(Sock pSock) {
		String hostname;
		InetAddress ipInfo;
		try {
			ipInfo = InetAddress.getLocalHost();
			hostname = ipInfo.getHostName();
			pSock.sendMsg(hostname);
		} catch (UnknownHostException e) {
			System.out.println("Failed connection: Unknown Client Host");
			return false;
		}
		String input = pSock.receiveMsg();
		if (input.equals("#")) {
			pSock.sendMsg("@");
			return true;
		} else {
			System.out.println("failed = " + input);
			return false;
		}
	}

	public static boolean sendCommands(Sock pSock, String[] args) {
		String cmd = "empty";
		if (args.length == 4) {
			cmd = "2 " + args[2] + " " + args[3]; //Case command is -l
		} else if (args.length == 5) {
			cmd = "3 " + args[2] + " " + args[3] + " " + args[4]; // Case Command is -g include filename
		}
		pSock.sendMsg(cmd);
		return true;
	}

	public static boolean processList(Sock pSock, Sock qSock, String[] args) {
		String buffer = pSock.receiveMsg();
		System.out.println("Receiving directory structure from " + args[0] + ":" + args[3]);
		if ("ack".equals(buffer)) {			// If server sends ok on plist it will send list on qSock
			while (!"%%".equals(buffer)) {
				buffer = qSock.receiveMsg();
				if (!"%%".equals(buffer)) {
					System.out.println(buffer);
				}
			}
		} else {
			System.out.println(buffer);//Else print error message sent by client
			return false;
		}
		return true;
	}

	public static boolean processFile(Sock pSock, Sock qSock, String[] args) {
		File file = new File(args[3]);
		int filelines  = 0;
		int count = 0;
		int fileCount = 1;
		String buffer;
		FileWriter fw = null;
/**
receiveMsg()
	

*/
		buffer = pSock.receiveMsg();//Receive ack or receive error
		if ("ack".equals(buffer)) {			// If server sends ok on plist it will send list on qSock
			buffer = pSock.receiveMsg();
			filelines = Integer.parseInt(buffer);
		} else {
			System.out.println(args[0] + ":" + args[4] + " says " + buffer); //Else print error message sent by client
			System.exit(0);
		}
		try {
			while (file.exists()) {
				file  = new File(args[3] + fileCount);
				fileCount++;
			}
			file.createNewFile();
			fw = new FileWriter(file.getName(), true);
			System.out.println("Receiving " + args[3] + " from " + args[0] + ":" + args[4]);
			while (count < filelines) {
				buffer = qSock.receiveMsg();
				fw.write(buffer + "\n");
				fw.flush();
				count++;
			}
			fw.close();
		} catch (IOException e) {
			System.out.println("File creation error.");
		}
		System.out.println("File transfer complete.");
		return true;
	}

	public static boolean checkForQSocket(Sock pSock) {
		String	buffer = pSock.receiveMsg();
		while (!"ack".equals(buffer)) {			// If server sends ok on pSock it will send list on qSock
			if ("err".equals(buffer)) {
				return false;
			}
			buffer = pSock.receiveMsg();
		}
		return true;
	}

	public static void main(String []args) {
		Sock pSock = new Sock();
		Sock qSock = new Sock();
		Scanner reader = new Scanner(System.in);
		String buffer = null;
		if (args.length >= 4 && args.length <= 5) {
			if (!verifyArgs(args)) {
				System.exit(0);
			}
		} else {
			System.out.println("Not enough arguments number of args = " + args.length + ".");
			System.exit(0);
		}
		pSock.createSocket(Integer.parseInt(args[1]), args[0]);
		handshake(pSock);
		sendCommands(pSock, args);
		if (checkForQSocket(pSock)) {

			if (args.length == 5) {
				qSock.createSocket(Integer.parseInt(args[4]), args[0]); //Case -g dataPort is arg[4]
			} else {
				qSock.createSocket(Integer.parseInt(args[3]), args[0]);//Case -l dataPort is arg[3]
			}
			if ("-l".equals(args[2])) {
				processList(pSock, qSock, args);
			} else if ("-g".equals(args[2])) {
				processFile(pSock, qSock, args);
			}
		}
		pSock.closeSocket();
		qSock.closeSocket();
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

	String receiveMsg() {
		//System.out.println("Waiting to receive!");
		try {
			in.ready();
			String str = in.readLine();
			//System.out.println("Text received: " + str);
			return str;
		} catch (IOException e) {
			System.out.println("Read failed");
			return null;
		}
	}

	void closeSocket() {
		try {
			this.socket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}


	}
}
