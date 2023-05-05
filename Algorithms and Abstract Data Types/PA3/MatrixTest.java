//-----------------------------------------------------------------------------
//  MatrixTest.java
//  A test client for the Matrix ADT
//-----------------------------------------------------------------------------

public class MatrixTest{
   public static void main(String[] args){
      //int i, j, n=100000;
      Matrix A;//= new Matrix(n);
      Matrix B; //= new Matrix(n);

      // A.changeEntry(1,1,1); B.changeEntry(1,1,1);
      // A.changeEntry(1,2,2); B.changeEntry(1,2,0);
      // A.changeEntry(1,3,3); B.changeEntry(1,3,1);
      // A.changeEntry(2,1,4); B.changeEntry(2,1,0);
      // A.changeEntry(2,2,5); B.changeEntry(2,2,1);
      // A.changeEntry(2,3,6); B.changeEntry(2,3,0);
      // A.changeEntry(3,1,7); B.changeEntry(3,1,1);
      // A.changeEntry(3,2,8); B.changeEntry(3,2,1);
      // A.changeEntry(3,3,9); B.changeEntry(3,3,1);


      // System.out.println(A.getNNZ());
      // System.out.println(A);

      // System.out.println(B.getNNZ());
      // System.out.println(B);

      // System.out.println("scalar");
      // //System.out.println(A);
      // Matrix C = A.scalarMult(1.5);
      // //System.out.println(A);
      // System.out.println(C.getNNZ());
      // System.out.println(C);
      // //System.out.println(A);

      // System.out.println("add");
      // System.out.println(A);
      // //System.out.println(B);
      // Matrix D = A.add(A);
      // System.out.println(D.getNNZ());
      // System.out.println(D);
      // //System.out.println(A);
      // //System.out.println(B);

      // System.out.println("sub");
      // //System.out.println(A);
      // //System.out.println(B);
      // Matrix E = B.sub(A);
      // System.out.println(E.getNNZ());
      // System.out.println(E);

      // System.out.println("transpose");
      // System.out.println(A);
      // Matrix F = A.transpose();
      // System.out.println(F.getNNZ());
      // System.out.println(F);
      // //System.out.println(A);

      // System.out.println("mult");
      // System.out.println(A);
      // System.out.println(B);
      // Matrix G = A.mult(B);
      // System.out.println(G.getNNZ());
      // System.out.println(G);

      // Matrix C;
      // A.changeEntry(1, 1, 4);
      // A.changeEntry(1, 2, 2);
      // A.changeEntry(1, 3, 0);
      // A.changeEntry(2, 1, 2);
      // A.changeEntry(3, 1, 0);
      // A.changeEntry(2, 2, 2);
      // A.changeEntry(3, 3, 0);
      // B.changeEntry(1, 1, -4);
      // B.changeEntry(1, 2, 0);
      // B.changeEntry(2, 1, 0);
      // B.changeEntry(2, 2, -2);
      // B.changeEntry(2, 4, 2);
      // B.changeEntry(3, 1, 0);
      // B.changeEntry(3, 2, 2);
      // B.changeEntry(7, 8, 5);
      // System.out.println(A);
      // System.out.println(B);
      // C = A.add(B);
      // System.out.println(C.getNNZ());
      // System.out.println(C);
                                          // 100*100 add add add test
      // Matrix C;
      // A = new Matrix(10);
      // B = new Matrix(15);
      // A.changeEntry(1, 1, 1);
      // B.changeEntry(1, 1, 1);
      // //if (A.equals(B)) return 1;
      // B = new Matrix(10);
      // A.changeEntry(1, 1, 1);
      // A.changeEntry(1, 3, 1);
      // B.changeEntry(1, 1, 1);
      // B.changeEntry(1, 3, 1);
      // //if (!A.equals(B)) return 2;
      // A.changeEntry(1, 3, 0);
      // //if (A.equals(B)) return 3;
      // A.changeEntry(1, 1, 0);
      // B.makeZero();
      // A.changeEntry(10, 10, 10);
      // B.changeEntry(10, 10, 10);
      // //if (!A.equals(B)) return 4;
      // A = new Matrix(100);
      // B = new Matrix(100);
      // int valcount = 1;
      // for (int j = 1; j <= 100; j++) {
      // for (int k = 1; k <= 100; k++) {
      // // hint: this is 1-10000 left-to-right row-by-row
      // A.changeEntry(j, k, valcount++);
      // }
      // B.changeEntry(j, j, 1); // hint: this is the identity matrix
      // }
      // C = A.scalarMult(2);
      // //if (!C.equals(A.add(A))) return 5;
      // //C = A.scalarMult(-2);
      // System.out.println(A.sub(A).add(A).add(A));
      // //System.out.println(C);
      // System.out.println(C.equals(A.sub(A).add(A).add(A)));




                                          //100*100 sub sub sub test
      // Matrix C;
      // A = new Matrix(10);
      // B = new Matrix(15);
      // A.changeEntry(1, 1, 1);
      // B.changeEntry(1, 1, 1);
      // //if (A.equals(B)) return 1;
      // B = new Matrix(10);
      // A.changeEntry(1, 1, 1);
      // A.changeEntry(1, 3, 1);
      // B.changeEntry(1, 1, 1);
      // B.changeEntry(1, 3, 1);
      // //if (!A.equals(B)) return 2;
      // A.changeEntry(1, 3, 0);
      // //if (A.equals(B)) return 3;
      // A.changeEntry(1, 1, 0);
      // B.makeZero();
      // A.changeEntry(10, 10, 10);
      // B.changeEntry(10, 10, 10);
      // //if (!A.equals(B)) return 4;
      // A = new Matrix(100);
      // B = new Matrix(100);
      // int valcount = 1;
      // for (int j = 1; j <= 100; j++) {
      // for (int k = 1; k <= 100; k++) {
      // // hint: this is 1-10000 left-to-right row-by-row
      // A.changeEntry(j, k, valcount++);
      // }
      // B.changeEntry(j, j, 1); // hint: this is the identity matrix
      // }
      // C = A.scalarMult(2);
      // //if (!C.equals(A.add(A))) return 5;
      // C = A.scalarMult(-2);
      // System.out.println(A.sub(A).sub(A).sub(A));
      // //System.out.println(C);
      // System.out.println(C.equals(A.sub(A).sub(A).sub(A)));

      // Matrix H = A.copy();
      // System.out.println(H.getNNZ());
      // System.out.println(H);
      // System.out.println(A.equals(H));
      // System.out.println(A.equals(B));
      // System.out.println(A.equals(A));

      // A.makeZero();
      // System.out.println(A.getNNZ());
      // System.out.println(A);

      A = new Matrix(10);
      A.changeEntry(1, 1, 1);
      A.changeEntry(1, 2, 0);
      A.changeEntry(1, 3, 3.5);
      A.changeEntry(5, 7, 2.5);
      A.changeEntry(3, 4, 2);
      A.changeEntry(1, 1, 2);
      System.out.println(A.toString());
      System.out.println("1: (1, 2.0) (3, 3.5)\n3: (4, 2.0)\n5: (7, 2.5)\n".replaceAll("\\s+",""));
      System.out.println(!A.toString().replaceAll("\\s+","").equals("1: (1, 2.0) (3, 3.5)\n3: (4, 2.0)\n5: (7, 2.5)\n".replaceAll("\\s+","")));
   }
}
