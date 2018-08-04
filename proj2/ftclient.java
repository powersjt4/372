import java.util.Scanner;
 
public class ftclient{

	public static int verifyArgs(String[] args){
		String edu = "engr.oregonstate.edu";
		System.out.println("edu " + edu + " args[0] "+ args[0]);
//		if(args[0] != "a"|| args[0] != "flip1"+ edu || args[0] != "flip2"+ edu || args[0] != "flip3"+edu || args[0] != "localhost"){
		if(!args[0].equals("a")){
			System.out.println("Incorrect server host.(flip(1,2,3) or localhost");
			return -1;
		}
		return 1;
	}

	public static void main(String []args) {
		if (args.length == 6){
			if(verifyArgs(args) < 0)
				System.exit(0);				
		}else{
			System.out.println("Not enough arguments.");
		}

		Scanner input  = new Scanner( System.in );
	
   }
	
}

