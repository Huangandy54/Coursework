/*
Andy Huang
ahuang44
PA1
*/

class List{
	private class Node{
		int data;
		Node next;
		Node previous;
		Node(int data){
			this.data=data;
			next=null;
			previous=null;
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
	// Returns front element. Pre: length()>0
	int front(){
		if (length==0) {
			throw new RuntimeException("List Error: front() called on empty List");
		}
		return front.data;
	}
	// Returns back element. Pre: length()>0
	int back(){
		if (length==0) {
			throw new RuntimeException("List Error: back() called on empty List");
		}
		return back.data;
	}
	// Returns cursor element. Pre: length()>0, index()>=0
	int get(){
		if (length==0) {
			throw new RuntimeException("List Error: get() called on empty List");
		}
		if (index()!=-1) {
			return cursor.data;
		}else{
			throw new RuntimeException("List Error: get() called on cursor that is off the list");
		}
	}
	//// Returns true if and only if this List and L are the same integer sequence
	boolean equals(List L){
		boolean isEqual=true;
		Node thisTemp=this.front;
		Node LTemp=L.front;
		if (this.length==L.length) {
			while(isEqual&&thisTemp!=null&&LTemp!=null){
				if (thisTemp.data!=LTemp.data) {
					isEqual=false;
					return false;
				}else{
					thisTemp=thisTemp.next;
					LTemp=LTemp.next;
				}
			}
			return true;
		}else{
			return false;
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
	void prepend(int data){
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
	void append(int data){
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
	void insertBefore(int data){
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
	void insertAfter(int data){
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
			fullList+=temp.data+" ";
			temp=temp.next;
		}
		return fullList;
	}
	
	// Returns a new List representing the same integer sequence as this List. The cursor in the new list is undefined, regardless of the state of the cursor in this List. This List is unchanged.
	List copy(){
		List listCopy=new List();
		Node temp=front;
		while (temp!=null) {
			listCopy.append(temp.data);
			temp=temp.next;
		}
		listCopy.cursor=null;
		return listCopy;
	}


}//end list class
