/*
Andy Huang
ahuang44
PA5
Graph.c 
*/
#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"


typedef struct GraphObj{
    //# of verts
    int order;
    //# of edges
    int size;
    
    List* adjList;
    int* color;
    int* parent;
    int* discover;
    int* finish;
}GraphObj;


/*** Constructors-Destructors ***/
Graph newGraph(int n){
    Graph G = malloc(sizeof(GraphObj));
    G->order=n;
    G->size=0;
    //adj list 
    G->adjList=calloc(n+1,sizeof(List));
    G->color=calloc(n+1,sizeof(int));
    G->parent=calloc(n+1,sizeof(int));
    G->discover=calloc(n+1,sizeof(int));
    G->finish=calloc(n+1,sizeof(int));
    for(int i=1; i<=n; i++){
        G->adjList[i]=newList();
        G->color[i]=WHITE;
        G->parent[i]=NIL;
        G->discover[i]=UNDEF;
        G->finish[i]=UNDEF;
    }
    return G;
}
void freeGraph(Graph* pG){
    for (int i = 0; i < getOrder(*pG); ++i)
    {
        freeList(&(*pG)->adjList[i]);
    }
    free((*pG)->adjList);
    free((*pG)->color);
    free((*pG)->parent);
    free((*pG)->discover);
    free((*pG)->finish);
    free(*pG);
    *pG=NULL;

}
/*** Access functions ***/
int getOrder(Graph G){
    return G->order;
}
int getSize(Graph G){
    return G->size;
}
int getParent(Graph G, int u){
    if (u<1||u>getOrder(G))
    {
        fprintf(stderr, "Out of bounds index");
        exit(EXIT_FAILURE);
    }
    return G->parent[u];
}
int getDiscover(Graph G, int u){
    return G->discover[u];
}
int getFinish(Graph G, int u){
    return G->finish[u];
}

/*** Manipulation procedures ***/
void makeNull(Graph G){
    for(int i=1; i<=getOrder(G); i++){
        clear(G->adjList[i]);
    }
}
void addEdge(Graph G, int u, int v){
    addArc(G,u,v);
    addArc(G,v,u);
    G->size--;
    
}
void addArc(Graph G, int u, int v){
    List L=G->adjList[u];
    moveFront(L);
    //empty list so just append
    if(length(L)==0){
      append(L,v);
    }else{
        //while cursor not out of bound and v is bigger than than cursor element move next
        while(index(L)!=-1&&v>get(L)){
            moveNext(L);
            if (index(L)==-1)
            {
                break;
            }
        }
        //if cursor not off list, insertbefore cursor since v is now less than the element at L's cursor.
        if (index(L)!=-1)
        {
            insertBefore(L,v);
        }else{
            append(L,v);
        }
    }
    
    G->size++;
}
void visit(Graph G,List S, int u,int *time){
    G->color[u]=GRAY;
    G->discover[u]=++*time;
    for (moveFront(G->adjList[u]);index(G->adjList[u])!=-1;moveNext(G->adjList[u]))
    {
        int temp=get(G->adjList[u]);
        if (G->color[temp]==WHITE)
        {
            G->parent[temp] = u;
            visit(G,S,temp,time);
        }
    }
    G->color[u]=BLACK;
    G->finish[u]=++*time;
    //prepend back to the List after finishing. Lower finish time = lower on the list after more are prepended to the list
    prepend(S,u);
}
void DFS(Graph G, List S){
    //fprintf(stderr, "DFS CALLED --------> length of S: %d, order of G: %d",length(S),G->order);
    if(length(S) != G->order) {
        //fprintf(stderr, "length of S: %d, order of G: %d",length(S),G->order);
        fprintf(stderr, "Graph error: DFS() must be called on S with length equal to order of G\n");
        exit(1);
    }
    //set up
    for(int i = 1; i <= getOrder(G); i++){
        G->color[i] = WHITE;
        G->parent[i] = NIL;
    }
    int time=0;
    //start at the front of the List. this is the order in which things will be processed
    moveFront(S);
    while(index(S)!=-1){
        int temp=get(S);
        if (G->color[temp]==WHITE)
        {
            visit(G,S,temp,&time);
        }
        moveNext(S);
    }
    // delete back of list to get rid of the original input since we now have the decreasing order of finish times in the List.
    for (int i = 0; i < getOrder(G); i++)
    {
        deleteBack(S);
    }
}
/*** Other operations ***/
Graph transpose(Graph G){
    Graph transpose = newGraph(getOrder(G));
    for(int i = 1; i <= getOrder(G); i++) {
        moveFront(G->adjList[i]);
        if(length(G->adjList[i]) != 0) {
            while(index(G->adjList[i]) != -1) {
                addArc(transpose, get(G->adjList[i]), i);
                moveNext(G->adjList[i]);
            }
        }
    }
    return transpose;
}
Graph copyGraph(Graph G);
void printGraph(FILE* out, Graph G){
    for(int i=1; i<= G->order; i++){
        fprintf(out,"%d:",i);
        printList(out,G->adjList[i]);
        fprintf(out,"\n");
    }
}