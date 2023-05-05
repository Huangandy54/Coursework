/*
Andy Huang
ahuang44
PA5
List.c 
*/

#include <stdio.h>
#include <stdlib.h>
#include "List.h"

typedef struct NodeObj{
	int data;
	struct NodeObj* next;
	struct NodeObj* previous;

}NodeObj;

typedef NodeObj* Node;

Node newNode(int data){
	Node S = malloc(sizeof(NodeObj));
	S->data = data;
	S->next = NULL;
	S->previous = NULL;
	return(S);

}

void freeNode(Node* pN){
	if(pN!=NULL && *pN!=NULL){
    	free(*pN);
    	*pN=NULL;
   	}
}
// Constructors-Destructors ---------------------------------------------------
typedef struct ListObj{
	Node front;
	Node back;
	Node cursor;
	int length;
}ListObj;

List newList(void){
	List L = malloc(sizeof(ListObj));
	L->front=L->back=L->cursor=NULL;
	L->length=0;
	return(L);
}
void freeList(List* pL){
	if(pL!=NULL && *pL!=NULL){
    	while(length(*pL)!=0){
    		deleteFront(*pL);
    	}
     	free(*pL);
     	*pL=NULL;
   }
}
// Access functions -----------------------------------------------------------
int length(List L){
	return L->length;
}
int index(List L){
	Node N=L->front;
	int index=0;
	if (L->cursor==NULL)
	{
		return -1;
	}
	while(N!=NULL){
		if (N->data==L->cursor->data)
		{
			return index;
		}
		N=N->next;
		index++;
	}
	return index;
}
int front(List L){
	if (L->length==0)
	{
		fprintf(stderr,"List Error: front() called on empty List");
		exit(EXIT_FAILURE);
	}
	return L->front->data;
}
int back(List L){
	if (L->length==0)
	{
		fprintf(stderr,"List Error: back() called on empty List");
		exit(EXIT_FAILURE);
	}
	return L->back->data;
}
int get(List L){
	if (L->length==0)
	{
		fprintf(stderr,"List Error: get() called on empty List");
		exit(EXIT_FAILURE);
	}
	if (index(L)!=-1)
	{
		return L->cursor->data;
	}else{
		fprintf(stderr,"List Error: back() called on cursor that is off the list");
		exit(EXIT_FAILURE);
	}
}
int equals(List A, List B){
	//1 is true 0 is false
	int returnValue=1;
	Node tempA=A->front;
	Node tempB=B->front;
	if (A->length==B->length)
	{
		while(returnValue==1&&tempA!=NULL&&tempB!=NULL){
			if (tempA->data!=tempB->data)
			{
				returnValue=0;
				return returnValue;
			}else{
				tempA=tempA->next;
				tempB=tempB->next;
			}
		}
		return 1;
	}else{
		return 0;
	}
}
// Manipulation procedures ----------------------------------------------------
void clear(List L){
	//L->front = L->back = L->cursor = NULL;
	while(length(L)!=0){
		deleteFront(L);
	}
	L->length=0;
}
void moveFront(List L){
	if (L->length!=0)
	{
		L->cursor=L->front;
	}
}
void moveBack(List L){
	if (L->length!=0)
	{
		L->cursor=L->back;
	}
}
void movePrev(List L){
	if (L->cursor!=NULL)
	{
		if (L->cursor==L->front)
		{
			L->cursor=NULL;
		}else{
			L->cursor=L->cursor->previous;
		}
	}
}
void moveNext(List L){
	if (L->cursor!=NULL)
	{
		if (L->cursor==L->back)
		{
			L->cursor=NULL;
		}else{
			L->cursor=L->cursor->next;
		}
	}
}
void prepend(List L, int data){
	//fprintf(stderr, "length of L is %d",length(L));
	Node temp=newNode(data);
	temp->next = L->front;
	temp->previous = NULL;
	if (L->front==NULL)
	{
		L->back=temp;
	}else{
		L->front->previous=temp;
	}
	L->front=temp;
	L->length++;
	//freeNode(&temp);
}
void append(List L, int data){
	Node temp=newNode(data);
	temp->previous = L->back;
	temp->next = NULL;
	if (L->back==NULL)
	{
		L->front=temp;
	}else{
		L->back->next=temp;
	}
	L->back=temp;
	L->length++;
	
}
void insertBefore(List L, int data){
	if (L->length==0)
	{
		fprintf(stderr,"List Error: insertBefore() called on empty List");
		exit(EXIT_FAILURE);
	}
	if (index(L)<0)
	{
		fprintf(stderr,"List Error: insertBefore() called on cursor that is off the list");
		exit(EXIT_FAILURE);
	}
	if(L->cursor==L->front){
		prepend(L,data);
	}else{
		Node temp=newNode(data);
		temp->previous=L->cursor->previous;
		temp->next=L->cursor;
		L->cursor->previous->next=temp;
		L->cursor->previous=temp;
		L->length++;
		//freeNode(&temp);
	}
}
void insertAfter(List L, int data){
	if (L->length==0)
	{
		fprintf(stderr,"List Error: insertAfter() called on empty List");
		exit(EXIT_FAILURE);
	}
	if (index(L)<0)
	{
		fprintf(stderr,"List Error: insertAfter() called on cursor that is off the list");
		exit(EXIT_FAILURE);
	}
	if(L->cursor==L->back){
		append(L,data);
	}else{
		Node temp=newNode(data);
		temp->next=L->cursor->next;
		temp->previous=L->cursor;
		L->cursor->next->previous=temp;
		L->cursor->next=temp;
		L->length++;
		//freeNode(&temp);
	}
}
void deleteFront(List L){
	if (L->length==0)
	{
		fprintf(stderr,"List Error: deleteFront() called on empty List");
		exit(EXIT_FAILURE);
	}
	if (index(L)==0)
	{
		L->cursor=NULL;
	}
	Node temp = L->front;
    if(length(L) > 1){
    	L->front = L->front->next;
    } 
    else {
    	L->front = L->back = NULL;
    }
    L->length--;
    freeNode(&temp);
	
}
void deleteBack(List L){
	if (L->length==0)
	{
		fprintf(stderr,"List Error: deleteBack() called on empty List");
		exit(EXIT_FAILURE);
	}
	if (index(L)==length(L)-1)
	{
		L->cursor=NULL;
	}
	Node temp = L->back;
    if(length(L) > 1){
    	L->back = L->back->previous;
    } 
    else {
    	L->back = L->front = NULL;
    }
    L->length--;
    freeNode(&temp);
	// if (L->length>1)
	// {
	// 	L->front=L->back=NULL;
	// 	freeNode(&L->front);
	// 	//L->length--;
	// }else{
	// 	Node temp=L->back;
	// 	L->back=temp->previous;
	// 	freeNode(&temp);
	// }
	
	// L->length--;
}
void delete(List L){
	if (L->length==0)
	{
		fprintf(stderr,"List Error: delete() called on empty List");
		exit(EXIT_FAILURE);
	}
	if (index(L)<0)
	{
		fprintf(stderr,"List Error: delete() called on cursor that is off the list");
		exit(EXIT_FAILURE);
	}
	if (L->cursor==L->front)
	{
		deleteFront(L);
	}else if(L->cursor==L->back){
		deleteBack(L);
	}else{
		L->cursor->previous->next=L->cursor->next;
		L->cursor->next->previous=L->cursor->previous;
		freeNode(&L->cursor);
		L->cursor=NULL;
		L->length--;
	}
}
// Other operations -----------------------------------------------------------
void printList(FILE* out, List L){
	Node temp=L->front;
	while(temp!=NULL){
		fprintf(out," %d",temp->data);
		temp=temp->next;
	}
}
List copyList(List L){
	List listCopy = newList();
	Node temp=L->front;
	while(temp!=NULL){
		append(listCopy,temp->data);
		temp=temp->next;
	}
	listCopy->cursor=NULL;
	return listCopy;
}