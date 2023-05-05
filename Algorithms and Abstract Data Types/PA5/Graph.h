/*   
Andy Huang
ahuang44
PA5
Graph.h
header file for Graph ADT
*/
#define UNDEF -10
#define NIL -1
#define WHITE 1
#define GRAY 2
#define BLACK 3
#include "List.h"

typedef struct GraphObj* Graph;
/*** Constructors-Destructors ***/
Graph newGraph(int n);
void freeGraph(Graph* pG);
/*** Access functions ***/
int getOrder(Graph G);
int getSize(Graph G);
int getParent(Graph G, int u);
int getDiscover(Graph G, int u); 
int getFinish(Graph G, int u); 
/*** Manipulation procedures ***/
void makeNull(Graph G);
void addEdge(Graph G, int u, int v);
void addArc(Graph G, int u, int v);
void DFS(Graph G, List S);
/*** Other operations ***/
Graph transpose(Graph G);
Graph copyGraph(Graph G);
void printGraph(FILE* out, Graph G);