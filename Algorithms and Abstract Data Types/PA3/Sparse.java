/*
Andy Huang
ahuang44
PA3
Sparse.java includes all the required functions listed in the pdf. Reads input and produces output.
Does operations that were given in pdf. 

*/
import java.io.*;
import java.util.Scanner;

public class Sparse{
    public static void main(String[] args)throws IOException{
        if(args.length != 2){
            System.out.println("Sparse needs <input file> <output file>");
            System.exit(1);
        }
        // open input file
        Scanner in = new Scanner(new File(args[0]));
        // output file
        PrintWriter out = new PrintWriter(new FileWriter(args[1]));
        int nLines=0;
        int sizeN=0;
        int ANum=0;
        int BNum=0;
        Matrix A=new Matrix(1);
        Matrix B=new Matrix(1);
        while(in.hasNextLine()){
            nLines++;
            // trim leading and trailing spaces, removed the colon.
            String line = in.nextLine()+ " "; 
            // split line around white space 
            String[] token = line.split("\\s+");
            if (nLines==1) {
                sizeN=Integer.parseInt(token[0]);
                ANum=Integer.parseInt(token[1]);
                BNum=Integer.parseInt(token[2]);
                A=new Matrix(sizeN);
                B=new Matrix(sizeN);            
            }
            else if(nLines>2){
                if (nLines<=ANum+2) {
                    int row=Integer.parseInt(token[0]);
                    int col=Integer.parseInt(token[1]);
                    double val=Double.parseDouble(token[2]);
                    A.changeEntry(row,col,val);
                }
                else if (nLines>ANum+3) {
                    int row=Integer.parseInt(token[0]);
                    int col=Integer.parseInt(token[1]);
                    double val=Double.parseDouble(token[2]);
                    B.changeEntry(row,col,val);
                }
            }
        }//end while loop for reading file
        // did it add correctly
        System.out.println(A+" "+A.getNNZ());
        System.out.println(B+" "+B.getNNZ());

        //Printing and Matrix ops
        //Printing A info
        out.println("A has "+A.getNNZ()+" non-zero entries:");
        out.println(A);
        //Printing B info
        out.println("B has "+B.getNNZ()+" non-zero entries:");
        out.println(B);
        //(1.5)*A
        out.println("(1.5)*A =");
        Matrix ScalarA=A.scalarMult(1.5);
        out.println(ScalarA);
        //A+B= 
        out.println("A+B =");
        Matrix addAB=A.add(B);
        out.println(addAB);
        //A+A= 
        out.println("A+A =");
        Matrix addAA=A.add(A);
        out.println(addAA);
        //B-A= 
        out.println("B-A =");
        Matrix BsubA=B.sub(A);
        out.println(BsubA);
        //A-A =
        out.println("A-A =");
        Matrix AsubA=A.sub(A);
        out.println(AsubA);
        //Transpose(A) =
        out.println("Transpose(A) =");
        Matrix transA=A.transpose();
        out.println(transA);
        //A*B
        out.println("A*B =");
        Matrix multAB=A.mult(B);
        out.println(multAB);
        //B*B
        out.println("B*B =");
        Matrix multBB=B.mult(B);
        out.println(multBB);


        in.close();
        out.close();
    }

}