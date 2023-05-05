/*
Andy Huang
ahuang44
PA1
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
		int nLines=0;
		//make a big string of every line in the file to be split later into a token array of strings
		String temp="";
		while(in.hasNextLine()){
			temp+=in.nextLine()+" ";
		}
		//splits line by white space to get
		String[] inputArray = temp.split("\\s+");
		//start the sorting using list ADT
		List list=new List();
		list.append(0);

		for (int x=1;x<inputArray.length;x++ ) {

			list.moveFront();
			System.out.println(list.get());
			while(inputArray[x].compareTo(inputArray[list.get()])>=0/*&&x-1!=inputArray.length*/){
				System.out.println(list.get());
				list.moveNext();
				System.out.println("cursor moved to next");
				if (list.index()==-1) {
					break;
				}

			}
			if (list.index()!=-1) {
				list.insertBefore(x);
				System.out.println("inserted "+x);
			}else{
				list.append(x);
				System.out.println("appended "+x);
			}




			// if (inputArray[x].compareTo(inputArray[list.get()])<0) {
			// 	System.out.println(inputArray[x]+"comes before"+inputArray[list.get()]);
			// }
		}
		// print to output file
		list.moveFront();
		for (int i=0;i<inputArray.length ;i++ ) {
			out.println(inputArray[list.get()]);
			System.out.println("printing "+inputArray[list.get()]);
			list.moveNext();
		}
		//System.out.println(inputArray[1].compareTo(inputArray[0]));
		System.out.println(list);
		//close files
		in.close();
		out.close();
	}

}