/*
Andy Huang
ahuang44
PA3
Matrix.java uses List.java to make matrices. 
Lists are the rows of the matrix and inside the list we hold the private class Entry from Matrix.java
Entry represents the col and the value.

*/
class Matrix{
    private class Entry{
        int col;
        double val;
        Entry(int col, double val){
            this.col=col;
            this.val=val;
            
        }
        public boolean equals(Object x){
            //System.out.println("called Entry.equals()");
            boolean isEqual=true;
            if (x instanceof Entry) {
                Entry temp=(Entry) x;
                //System.out.println("this.col: "+this.col+", temp.col: "+temp.col+"this.val: "+this.val+", temp.val: "+temp.val);
                if ((this.col==temp.col&&this.val==temp.val)==false) {
                    //System.out.println("Entry equals returning false");
                    isEqual=false;
                }
            }
            return isEqual;
        }
        public String toString(){
            return(" ("+this.col+", "+this.val+")");
        }
    }
    // Constructor
    private List[] row;
    private int size;
    private int nnz=0;
    // Makes a new n x n zero Matrix. pre: n>=1
    Matrix(int n){
        size=n;
        row=new List[n+1];
        for (int i=1;i<=n ;i++ ) {
            row[i]=new List();
        }
    }
    // Access functions
    // Returns n, the number of rows and columns of this Matrix
    int getSize(){
        return size;
    }
    // Returns the number of non-zero entries in this Matrix
    int getNNZ(){
        return nnz;
    }
    // overrides Object's equals() method || calls to List.equals()
    public boolean equals(Object x){
        //System.out.println("called Matrix.equals()");
        boolean isEqual=true;
        if (x instanceof Matrix) {
            Matrix temp=(Matrix) x;
            if (this.size==temp.size) {
                for (int i=1;i<=size;i++) {
                    if (!(this.row[i].equals(temp.row[i]))) {
                        //System.out.println("Matrix equals returning false 1");
                        isEqual=false;
                        return isEqual;
                    }
                }
            }
            else{
                //System.out.println("Matrix equals returning false 2");
                isEqual=false;
                return isEqual;
            }
        }
        
        return isEqual;
    }
    // Manipulation procedures 

    // sets this Matrix to the zero state
    void makeZero(){
        for (int i=1;i<=size;i++ ) {
            this.row[i]=new List();
        }
        this.nnz=0;
    }
    // returns a new Matrix having the same entries as this Matrix
    Matrix copy(){
        Matrix tempMatrix=new Matrix(size);
        for (int i=1;i<=size ;i++ ) {
            this.row[i].moveFront();
            // goes from front to back of row(List) 
            while(this.row[i].index()>=0){
                // get entry object from row[i] using cursor and add it to the temp Matrix
                Entry tempEntry=(Entry)this.row[i].get();
                tempMatrix.row[i].append(new Entry(tempEntry.col,tempEntry.val));
                this.row[i].moveNext();
            }
        }
        tempMatrix.nnz=this.nnz;
        return tempMatrix;
    }
    // changes ith row, jth column of this Matrix to x
    // pre: 1<=i<=getSize(), 1<=j<=getSize()
    void changeEntry(int i, int j, double x){
        if (i<1||i>size||j<1||j>size) {
            throw new RuntimeException("Matrix Error: invalid args: 1<=i<=getSize(), 1<=j<=getSize()");
        }
        //move cursor to front of row "i"
        this.row[i].moveFront();
        Entry newEntry=new Entry(j,x);
        //System.out.println(row[i].length()+" doing ("+i+", "+j+", "+x+")");
        // case 1 if A is empty and x is 0 --> do nothing
        if (this.row[i].length()==0&&x==0) {
            //System.out.println("case 1");
            return;
        }
        // case 2 if A is empty and x!=0 --> insert
        if (this.row[i].length()==0&&x!=0) {
            //System.out.println("case 2");
            this.row[i].append(newEntry);
            this.nnz++;
            return;
        }
        // case 3 if A is not empty and x==0 --> delete Entry at col j
        if (this.row[i].length()!=0&&x==0) {
            //System.out.println("case 3");
            while(this.row[i].index()>=0){
                Entry tempEntry=(Entry)this.row[i].get();
                if (tempEntry.col==j) {
                    this.row[i].delete();
                    this.nnz--;
                    return;
                }
                this.row[i].moveNext();
            }
        }
        // case 4 if A is not empty and x !=0 --> find entry at col and change val
        if (this.row[i].length()!=0&&x!=0) {
            //System.out.println("case 4");
            //System.out.println(row[i].index());
            //Try to find if col already exist
            while(this.row[i].index()>=0){
                //Entry tempEntry=(Entry)this.row[i].get();
                if (((Entry)this.row[i].get()).col==j) {
                    ((Entry)this.row[i].get()).val=x;
                    return;
                }
                //System.out.println(row[i].index());
                this.row[i].moveNext();
            }
            // Does not already exist so move front and look for place to insert
            // Same logic as Lex.java but this time inserting based on col number
            this.row[i].moveFront();
            while(((Entry)this.row[i].get()).col<j){
                this.row[i].moveNext();
                if (this.row[i].index()==-1) {
                    break;
                }
            }
            //System.out.println("ended at index: "+row[i].index());
            if (this.row[i].index()!=-1) {
                //System.out.println("inserting before");
                this.row[i].insertBefore(newEntry);
                this.nnz++;
                return;
            }else{
                this.row[i].append(newEntry);
                this.nnz++;
                return;
            }
        }
    }
    // returns a new Matrix that is the scalar product of this Matrix with x
    Matrix scalarMult(double x){
        Matrix temp=this.copy();
        //System.out.println(temp);
        //System.out.println(temp);
        for (int i=1;i<=size ;i++ ) {
            temp.row[i].moveFront();
            
            while (temp.row[i].index()>=0) {
                //System.out.println("before: "+temp);
                temp.changeEntry(i,((Entry)temp.row[i].get()).col,x*((Entry)temp.row[i].get()).val);
                //((Entry)temp.row[i].get()).val=((Entry)temp.row[i].get()).val*x;
                //System.out.println("changed "+i+" "+((Entry)temp.row[i].get()).col+" "+((Entry)temp.row[i].get()).val);
                //System.out.println("After: "+temp);
                temp.row[i].moveNext();
            }
        }
        //System.out.println("about to return"+temp);
        return temp;
    }
    // returns a new Matrix that is the sum of this Matrix with M
    // pre: getSize()==M.getSize()
    Matrix add(Matrix M){
        if (M.getSize()!=getSize()) {
            throw new RuntimeException("Matrix Addition Error: matrix sizes aren't the same");
        }
        //adding 2 same matrix = double of one
        Matrix temp=new Matrix(size);
        if (this.equals(M)) {
            //System.out.println("before: "+temp);
            temp=this.scalarMult(2.0);
            //System.out.println("After: "+temp);
            temp.nnz=this.nnz;
            return temp;
        }
        for (int i=1;i<=size;i++ ) {
            this.row[i].moveFront();
            M.row[i].moveFront();
            //A row is not empty but B row is so copy A row to temp
            if (this.row[i].index()>=0&&M.row[i].index()==-1) {
                while(this.row[i].index()>=0){
                    temp.row[i].append(new Entry(((Entry)this.row[i].get()).col,((Entry)this.row[i].get()).val));
                    temp.nnz++;
                    this.row[i].moveNext();
                }
            }//A row is empty but B row isnt so copy B row to temp
            else if (this.row[i].index()==-1&&M.row[i].index()>=0) {
                while(M.row[i].index()>=0){
                    temp.row[i].append(new Entry(((Entry)M.row[i].get()).col,((Entry)M.row[i].get()).val));
                    temp.nnz++;
                    M.row[i].moveNext();
                }
            }
            // both rows arent empty
            else{
                while(this.row[i].index()>=0||M.row[i].index()>=0){
                    //System.out.println(this.row[i].index());
                    //System.out.println(M.row[i].index());
                    //case 4 if A.get is null thus we've gone through all A but B isnt so insert B and increment B
                    if ((this.row[i].index())==-1&&((Entry)M.row[i].get())!=null) {
                        //System.out.println("case 4");
                        temp.row[i].append(new Entry(((Entry)M.row[i].get()).col,((Entry)M.row[i].get()).val));
                        temp.nnz++;
                        M.row[i].moveNext();
                    }
                    //case 5 if B.get is null thus we've gone through all B but A isnt so insert A and increment A
                    else if(((Entry)this.row[i].get())!=null&&(M.row[i].index())==-1){
                        //System.out.println("case 5");
                        temp.row[i].append(new Entry(((Entry)this.row[i].get()).col,((Entry)this.row[i].get()).val));
                        temp.nnz++;
                        this.row[i].moveNext();
                    }
                    //case 1 if A.col is < N.col. insert A Entry, increment cursor to next entry
                    else if (((Entry)this.row[i].get()).col<((Entry)M.row[i].get()).col){
                        //System.out.println("case 1");
                        temp.row[i].append(new Entry(((Entry)this.row[i].get()).col,((Entry)this.row[i].get()).val));
                        temp.nnz++;
                        this.row[i].moveNext();
                    }
                    //case 2 if A.col is > B.col. insert B Entry, increment cursor to next entry
                    else if (((Entry)this.row[i].get()).col>((Entry)M.row[i].get()).col){
                        //System.out.println("case 2");
                        temp.row[i].append(new Entry(((Entry)M.row[i].get()).col,((Entry)M.row[i].get()).val));
                        temp.nnz++;
                        M.row[i].moveNext();
                    }
                    //case 3 if A.col is = B.col. add val, increment both cursors to next entry
                    else if (((Entry)this.row[i].get()).col==((Entry)M.row[i].get()).col){
                        //System.out.println("case 3");
                        if (((Entry)this.row[i].get()).val+((Entry)M.row[i].get()).val==0) {
                            this.row[i].moveNext();
                            M.row[i].moveNext();
                        }else{
                            temp.row[i].append(new Entry(((Entry)this.row[i].get()).col,((Entry)this.row[i].get()).val+((Entry)M.row[i].get()).val));
                            temp.nnz++;
                            this.row[i].moveNext();
                            M.row[i].moveNext();
                        }
                        ;
                        //System.out.println("A: "+this.row[i].index());
                        //System.out.println("B: "+M.row[i].index());
                        
                    }
                    
                }   
            }
            
        }

        return temp;
    }
    // returns a new Matrix that is the difference of this Matrix with M
    // pre: getSize()==M.getSize()
    Matrix sub(Matrix M){
        if (M.getSize()!=getSize()) {
            throw new RuntimeException("Matrix Addition Error: matrix sizes aren't the same");
        }
        Matrix temp=new Matrix(size);
        // subtracting 2 same matrix = 0
        if (this.equals(M)) {
            return temp;
        }
        for (int i=1;i<=size;i++ ) {
            this.row[i].moveFront();
            M.row[i].moveFront();
            //A row is not empty but B row is so copy A row to temp
            if (this.row[i].index()>=0&&M.row[i].index()==-1) {
                while(this.row[i].index()>=0){
                    temp.row[i].append(new Entry(((Entry)this.row[i].get()).col,((Entry)this.row[i].get()).val));
                    temp.nnz++;
                    this.row[i].moveNext();
                }
            }//A row is empty but B row isnt so copy B row to temp but make values negative
            else if (this.row[i].index()==-1&&M.row[i].index()>=0) {
                while(M.row[i].index()>=0){
                    temp.row[i].append(new Entry(((Entry)M.row[i].get()).col,((Entry)M.row[i].get()).val*-1));
                    temp.nnz++;
                    M.row[i].moveNext();
                }
            }
            // both rows arent empty
            else{
                while(this.row[i].index()>=0||M.row[i].index()>=0){
                    //System.out.println(this.row[i].index());
                    //System.out.println(M.row[i].index());
                    //case 4 if A.get is null thus we've gone through all A but B isnt so insert B and increment B but make negative
                    if ((this.row[i].index())==-1&&((Entry)M.row[i].get())!=null) {
                        //System.out.println("case 4");
                        temp.row[i].append(new Entry(((Entry)M.row[i].get()).col,((Entry)M.row[i].get()).val*-1));
                        temp.nnz++;
                        M.row[i].moveNext();
                    }
                    //case 5 if B.get is null thus we've gone through all B but A isnt so insert A and increment A
                    else if(((Entry)this.row[i].get())!=null&&(M.row[i].index())==-1){
                        //System.out.println("case 5");
                        temp.row[i].append(new Entry(((Entry)this.row[i].get()).col,((Entry)this.row[i].get()).val));
                        temp.nnz++;
                        this.row[i].moveNext();
                    }
                    //case 1 if A.col is < N.col. insert A Entry, increment cursor to next entry
                    else if (((Entry)this.row[i].get()).col<((Entry)M.row[i].get()).col){
                        //System.out.println("case 1");
                        temp.row[i].append(new Entry(((Entry)this.row[i].get()).col,((Entry)this.row[i].get()).val));
                        temp.nnz++;
                        this.row[i].moveNext();
                    }
                    //case 2 if A.col is > B.col. insert B Entry, increment cursor to next entry but make val negative since its 0-B.val
                    else if (((Entry)this.row[i].get()).col>((Entry)M.row[i].get()).col){
                        //System.out.println("case 2");
                        temp.row[i].append(new Entry(((Entry)M.row[i].get()).col,((Entry)M.row[i].get()).val*-1));
                        temp.nnz++;
                        M.row[i].moveNext();
                    }
                    //case 3 if A.col is = B.col. calculate difference= A.val-B.val, if 0, do nothing else add too temp. increment both cursors to next entry
                    else if (((Entry)this.row[i].get()).col==((Entry)M.row[i].get()).col){
                        //System.out.println("case 3");
                        if (((Entry)this.row[i].get()).val-((Entry)M.row[i].get()).val==0) {
                            
                        }else{
                            temp.row[i].append(new Entry(((Entry)this.row[i].get()).col,((Entry)this.row[i].get()).val-((Entry)M.row[i].get()).val));
                            temp.nnz++;
                        }
                        
                        this.row[i].moveNext();
                        // System.out.println("A: "+this.row[i].index());
                        // System.out.println("B: "+M.row[i].index());
                        M.row[i].moveNext();
                    }
                    
                }   
            }
            
        }
        return temp;
    }
    // returns a new Matrix that is the transpose of this Matrix
    Matrix transpose(){
        Matrix temp=new Matrix(size);
        for(int i=1; i<=size; i++){
         this.row[i].moveFront();
         while(this.row[i].index()>=0){
            temp.changeEntry(((Entry)this.row[i].get()).col, i, ((Entry)this.row[i].get()).val);
            this.row[i].moveNext();
         }
      }
        return temp;
    }
    // returns a new Matrix that is the product of this Matrix with M
    // pre: getSize()==M.getSize()
    Matrix mult(Matrix M){
        Matrix temp=new Matrix(size);
        Matrix MTrans= M.transpose();
        
        //System.out.println("Trans :\n"+MTrans);
        //System.out.println(dot(this.row[1],MTrans.row[1]));
        double x=0.0;
        for (int i =1;i<=size ;i++ ) {

            for (int j=1;j<=size ;j++ ) {
                x=0.0;
                this.row[i].moveFront();
                MTrans.row[j].moveFront();
                if (this.row[i].length()==0||MTrans.row[j].length()==0) {
                    //System.out.println("skip");
                    continue;
                }
                else{
                    while(this.row[i].index()!=-1 && MTrans.row[j].index()!=-1){
                        if (((Entry)this.row[i].get()).col==((Entry)MTrans.row[j].get()).col) {
                            x+=((Entry)this.row[i].get()).val*((Entry)MTrans.row[j].get()).val;
                            //System.out.println(x);
                            this.row[i].moveNext();
                            MTrans.row[j].moveNext();
                        }else if (((Entry)this.row[i].get()).col<((Entry)MTrans.row[j].get()).col) {
                            this.row[i].moveNext();
                        }else{
                            MTrans.row[j].moveNext();
                        }
                    }
                }
                
                //System.out.println("inserting: "+i+" "+j+" "+x);
                if (x!=0) {
                    temp.row[i].append(new Entry(j,x));
                    temp.nnz++; 
                }
                
                //System.out.println(dot(this.row[1],MTrans.row[1]));
            }
        }
        //System.out.println(temp.nnz);
        return temp;
    }
    // Other functions
    // overrides Object's toString() method
    public String toString(){
        String fullMat="";
        for (int i=1;i<=size;i++ ) {
            if (row[i].length()>0) {
                fullMat+=i+":"+row[i]+"\n";
            }
        }
        return fullMat;
    }
//   did not work
//  private static double dot(List P, List Q){
//      double total=0.0;
//      P.moveFront();
//      Q.moveFront();
//      while(P.index()!=-1&& Q.index()!=-1){
//          Entry pEntry=(Entry)P.get();
//          Entry qEntry=(Entry)Q.get();
//          // cols arent equal yet so move the one that is smaller
//          if(pEntry.col==qEntry.col){
//              System.out.println(total);
//              System.out.println("multiplying "+pEntry.val+"to "+qEntry.val+"= "+pEntry.val*qEntry.val);
//              total=total+(pEntry.val*qEntry.val);
//              System.out.println(total);
//              P.moveNext();
//              Q.moveNext();
//          }
//          // cols arent equal yet so move the one that is smaller
//          else if(pEntry.col<pEntry.col){
//              P.moveNext();
//          }
//          // cols are equal
//          else if(pEntry.col>pEntry.col){
//              Q.moveNext();
//              }
//          }
//          return total;
//      }
}

