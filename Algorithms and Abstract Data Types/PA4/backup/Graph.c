/*
Andy Huang
ahuang44
PA4
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
	int source;
	List* adjList;
	int* color;
	int* parent;
	int* distance;
}GraphObj;


/*** Constructors-Destructors ***/
Graph newGraph(int n){
	Graph G = malloc(sizeof(GraphObj));
	G->order=n;
	G->size=0;
	G->source=NIL;
	//adj list 
	G->adjList=calloc(n+1,sizeof(List));
	G->color=calloc(n+1,sizeof(int));
	G->parent=calloc(n+1,sizeof(int));
	G->distance=calloc(n+1,sizeof(int));
	for(int i=1; i<=n; i++){
		G->adjList[i]=newList();
		G->color[i]=WHITE;
		G->parent[i]=NIL;
		G->distance[i]=INF;
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
	free((*pG)->distance);
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
int getSource(Graph G){
	return G->source;
}
int getParent(Graph G, int u){
	if (u<1||u>getOrder(G))
	{
		fprintf(stderr, "Out of bounds index");
		exit(EXIT_FAILURE);
	}
	return G->parent[u];
}
int getDist(Graph G, int u){
	if (u<1||u>getOrder(G))
	{
		fprintf(stderr, "Out of bounds index");
		exit(EXIT_FAILURE);
	}
	if(getSource(G)==NIL){
		return INF;
   	}
	return G->distance[u];
}
void getPath(List L, Graph G, int u){
	if (u<1||u>getOrder(G))
	{
		fprintf(stderr, "Out of bounds index");
		exit(EXIT_FAILURE);
	}
	if (getSource(G)==NIL)
	{
		fprintf(stderr, "BFS() has not been called.");
		exit(EXIT_FAILURE);
	}
	//path to u is source
	if (G->source==u)
	{
		append(L,u);
	}
	//recursively call to the parent of u until we get to the source
	else if(G->parent[u]!=NIL){
		getPath(L,G,G->parent[u]);
		append(L,u);
	}
	//append NIL if no path
	else{
		append(L,NIL);
	}
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
   	}
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
	G->size++;
}
void BFS(Graph G, int s);
/*** Other operations ***/
void printGraph(FILE* out, Graph G);