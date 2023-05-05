/*
Andy Huang
ahuang44
PA1

Lex.java uses List.java to sort. It does not use any direct sorting methods but instead using the List ADT functions to properly place
things in the correct place. 
*/
import java.io.*;
import java.util.Scanner;

public class Lex{
	public static void main(String[] args)throws IOException{
		if(args.length != 2){
       		System.out.println("Lex needs <input file> <output file>");
       		System.exit(1);
   		}
   		// open input file
   		Scanner in = new Scanner(new File(args[0]));
      	// output file
      	PrintWriter out = new PrintWriter(new FileWriter(args[1]));
      	// counter for how many lines
		int nLines=0;
		// count how many lines in file
		while(in.hasNextLine()){
			nLines++;
			in.nextLine();
		}
		// close and reopen file
		in.close();
		in = new Scanner(new File(args[0]));
		// add all lines into the array
		String line;
		String[] inputArray = new String[nLines];
		for (int i = 0; i < nLines; i++){
	    	line = in.nextLine();   
	        inputArray[i] = line;
		}
		//start the sorting using list ADT
		List list=new List();
		list.append(0);

		for (int x=1;x<inputArray.length;x++ ) {

			list.moveFront();
			//System.out.println(list.get());
			while(inputArray[x].compareTo(inputArray[list.get()])>=0/*&&x-1!=inputArray.length*/){
				//System.out.println(list.get());
				list.moveNext();
				//System.out.println("cursor moved to next");
				if (list.index()==-1) {
					break;
				}

			}
			if (list.index()!=-1) {
				list.insertBefore(x);
				//System.out.println("inserted "+x);
			}else{
				list.append(x);
				//System.out.println("appended "+x);
			}


			

			// if (inputArray[x].compareTo(inputArray[list.get()])<0) {
			// 	System.out.println(inputArray[x]+"comes before"+inputArray[list.get()]);
			// }
		}
		// print to output file
		list.moveFront();
		for (int i=0;i<inputArray.length ;i++ ) {
			out.println(inputArray[list.get()]);
			// System.out.println("printing "+inputArray[list.get()]);
			list.moveNext();
		}
		//System.out.println(inputArray[1].compareTo(inputArray[0]));
		//System.out.println(list);
		//close files
		in.close();
		out.close();
	}

}