/*
Andy Huang
ahuang44
PA4
GraphClient.c 
simple tests
*/
#include<stdio.h>
#include<stdlib.h>
#include"Graph.h"

int main(int argc, char* argv[]){
   int c=argc;
   c=c;
   FILE *out = fopen(argv[1], "w");
   Graph G=newGraph(5);
   List L=newList();
   printf("%d",getOrder(G));
   addArc(G,1,1);
   addArc(G,1,2);
   addArc(G,1,3);
   addArc(G,3,4);
   addArc(G,3,5);
   addArc(G,1,5);
   printf("%d",getOrder(G));
   printf("%d",getSize(G));
   DFS(G,S);
   printList(out,L);
   printf("%d",getDist(G,4));
   printf("%d",getSource(G));
   printGraph(out,G);
   makeNull(G);
   printf("%d",getOrder(G));
   printGraph(out,G);
   addArc(G,1,5);
   addArc(G,1,4);
   addArc(G,1,3);
   addArc(G,3,2);
   addArc(G,3,1);
   addArc(G,1,2);
   printf("%d",getOrder(G));
   printf("%d",getSize(G));
   DFS(G,S);
   printList(out,L);
   printf("%d",getDist(G,4));
   freeGraph(&G);
}
