/*
Andy Huang
ahuang44
PA3
List.java includes all the required functions listed in the pdf. 

*/

class List{
    private class Node{
        Object data;
        Node next;
        Node previous;
        Node(Object data){
            this.data=data;
            next=null;
            previous=null;
        }
        //"calls Entry.equals()"
        public boolean equals(Object x){
            //System.out.println("node equals called");
            boolean isEqual=true;
            Node temp= (Node) x;
            //System.out.println("this.data: "+this.data.toString()+" temp.data: "+temp.data.toString());
            if (this.data.equals(temp.data)==false) {
                //System.out.println("node equals returning false");
                isEqual=false;
            }
            return isEqual;
        }
        //calls "Entry.toString"
        public String toString(){
            return data.toString();
        }
    }
    private Node front;
    private Node back;
    private Node cursor;
    private int length;
    List(){
        front=back=cursor=null;
        length=0;
    }

    // Returns the number of elements in this List.
    int length(){
        return length;
    }
    // If cursor is defined, returns the index of the cursor element, otherwise returns -1.
    int index(){
        Node n=front;
        int index=0;
        if (cursor==null) {
            return -1;
        }
        while(n!=null){
            if (n.data==cursor.data) {
                return index;
            }
            n=n.next;
            index++;
        }
        return index;
    }
    // calls to Node.equals()
    public boolean equals(Object x){
        //System.out.println("list equals called");
        boolean isEqual=true;
        List temp=(List)x;
        Node thisTemp=this.front;
        Node xTemp=temp.front;
        if (this.length==temp.length) {
            while(isEqual&&thisTemp!=null&&xTemp!=null){
                if (!(thisTemp.equals(xTemp))) {
                    //System.out.println("List equals returning false 1");
                    isEqual=false;
                    return false;
                }else{
                    thisTemp=thisTemp.next;
                    xTemp=xTemp.next;
                }
            }
            return true;
        }else{
            //System.out.println("List equals returning false 2");
            return false;
        }
    }
    // Returns front element. Pre: length()>0
    Object front(){
        if (length==0) {
            throw new RuntimeException("List Error: front() called on empty List");
        }
        return front.data;
    }
    // Returns back element. Pre: length()>0
    Object back(){
        if (length==0) {
            throw new RuntimeException("List Error: back() called on empty List");
        }
        return back.data;
    }
    // Returns cursor element. Pre: length()>0, index()>=0
    Object get(){
        if (length==0) {
            throw new RuntimeException("List Error: get() called on empty List");
        }
        if (index()!=-1) {
            return cursor.data;
        }else{
            throw new RuntimeException("List Error: get() called on cursor that is off the list");
        }
    }
    
    //Manipulation procedures
    // Resets this List to its original empty state.
    void clear(){
        front=back=cursor=null;
        length=0;
    }
    // If List is non-empty, places the cursor under the front element, otherwise does nothing.
    void moveFront(){
        if (length!=0) {
            cursor=front;

        }
    }
    // If List is non-empty, places the cursor under the back element, otherwise does nothing.
    void moveBack(){
        if (length!=0) {
            cursor=back;
        }
    }
    // If cursor is defined and not at front, moves cursor one step toward front of this List, if cursor is defined and at front, cursor becomes undefined, if cursor is undefined does nothing.
    void movePrev(){
        if (cursor!=null) {
            if (cursor==front) {
                cursor=null;
            }else{
                cursor=cursor.previous;
            }
        }
    }
    // If cursor is defined and not at back, moves cursor one step toward back of this List, if cursor is defined and at back, cursor becomes undefined, if cursor is undefined does nothing.
    void moveNext(){
        if (cursor!=null) {
            if (cursor==back) {
                cursor=null;
            }else{
                cursor=cursor.next;
            }
        }
    }
    // Insert new element into this List. If List is non-empty, insertion takes place before front element.
    void prepend(Object data){
        Node temp=new Node(data);
        if (front==null) {
            front=temp;
            back=temp;
            length++;
        }else{
            front.previous=temp;
            temp.next=front;
            front=temp;
            length++;
        }
    }

    // Insert new element into this List. If List is non-empty, insertion takes place after back element.
    void append(Object data){
        Node temp=new Node(data);
        if (back==null) {
            front=temp;
            back=temp;
            length++;
        }else{
            back.next=temp;
            temp.previous=back;
            back=temp;
            length++;
        }
    }
    // Insert new element before cursor. Pre: length()>0, index()>=0
    void insertBefore(Object data){
        if (length==0) {
            throw new RuntimeException("List Error: insertBefore() called on empty List");
        }
        if (index()<0) {
            throw new RuntimeException("List Error: insertBefore() called on cursor that is off the list");
        }
        if (cursor==front) {
            prepend(data);
        }else{
            Node temp=new Node(data);
            temp.previous = cursor.previous;
            temp.next = cursor;
            cursor.previous.next = temp;
            cursor.previous = temp;
            length++;
        }
    }
    // Inserts new element after cursor. Pre: length()>0, index()>=0
    void insertAfter(Object data){
        if (length==0) {
            throw new RuntimeException("List Error: insertAfter() called on empty List");
        }
        if (index()<0) {
            throw new RuntimeException("List Error: insertAfter() called on cursor that is off the list");
        }
        if (cursor==back) {
            append(data);
        }else{
            Node temp=new Node(data);
            temp.next=cursor.next;
            temp.previous=cursor;
            cursor.next.previous=temp;
            cursor.next=temp;
            length++;
        }
    }
    // Deletes the front element. Pre: length()>0
    void deleteFront(){
        if (length==0) {
            throw new RuntimeException("List Error: deleteFront() called on empty List");
        }
        if (index()==0) {
            cursor=null;
        }
        front=front.next;
        length--;
        
    }

    // Deletes the back element. Pre: length()>0
    void deleteBack(){
        if (length==0) {
            throw new RuntimeException("List Error: deleteBack() called on empty List");
        }
        if (index()==length-1) {
            cursor=null;
        }
        back=back.previous;
        length--;

    }

    // Deletes cursor element, making cursor undefined. Pre: length()>0, index()>=0
    void delete(){
        if (length==0) {
            throw new RuntimeException("List Error: delete() called on empty List");
        }
        if (index()<0) {
            throw new RuntimeException("List Error: delete() called on cursor that is off the list");
        }
        if (cursor==front) {
            deleteFront();
        }else if(cursor==back){
            deleteBack();
        }else{
            cursor.previous.next=cursor.next;
            cursor.next.previous=cursor.previous;
            cursor=null;
            length--;
        }
        
    }

    // Overrides Object's toString method. Returns a String representation of this List consisting of a space separated sequence of integers, with front on left.
    public String toString(){
        String fullList="";
        Node temp=front;
        while(temp!=null){
            if (temp.data instanceof Integer) {
                fullList+=temp.data+" ";
                temp=temp.next;
            }else{
            fullList+=temp.data;
            temp=temp.next; 
            }
            
        }
        return fullList;
    }
    // not needed for pa3
    // Returns a new List representing the same integer sequence as this List. The cursor in the new list is undefined, regardless of the state of the cursor in this List. This List is unchanged.
    // List copy(){
    //  List listCopy=new List();
    //  Node temp=front;
    //  while (temp!=null) {
    //      listCopy.append(temp.data);
    //      temp=temp.next;
    //  }
    //  listCopy.cursor=null;
    //  return listCopy;
    // }


}//end list class
