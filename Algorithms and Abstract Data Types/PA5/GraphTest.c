/*
Andy Huang
ahuang44
PA5
GraphTest.c 
*/
#include<stdio.h>
#include<stdlib.h>
#include"Graph.h"


int main(int argc, char* argv[]){
   int c=argc;
   c=c;
   FILE *out = fopen(argv[1], "w");
   int i, n=8;
   List S = newList();
   Graph G = newGraph(n);
   Graph T=NULL, C=NULL;

   for(i=1; i<=n; i++) append(S, i);

   addArc(G, 1,2);
   addArc(G, 1,5);
   addArc(G, 2,5);
   addArc(G, 2,6);
   addArc(G, 3,2);
   addArc(G, 3,4);
   addArc(G, 3,6);
   addArc(G, 3,7);
   addArc(G, 3,8);
   addArc(G, 6,5);
   addArc(G, 6,7);
   addArc(G, 8,4);
   addArc(G, 8,7);
   printGraph(out, G);

   DFS(G, S);
   fprintf(out, "\n");
   fprintf(out, "x:  d  f  p\n");
   for(i=1; i<=n; i++){
      fprintf(out, "%d: %2d %2d %2d\n", i, getDiscover(G, i), getFinish(G, i), getParent(G, i));
   }
   fprintf(out, "\n");
   printList(out, S);
   fprintf(out, "\n");

   T = transpose(G);
   C = copyGraph(G);
   fprintf(out, "\n");
   printGraph(out, C);
   fprintf(out, "\n");
   printGraph(out, T);
   fprintf(out, "\n");

   DFS(T, S);
   fprintf(out, "\n");
   fprintf(out, "x:  d  f  p\n");
   for(i=1; i<=n; i++){
      fprintf(out, "%d: %2d %2d %2d\n", i, getDiscover(T, i), getFinish(T, i), getParent(T, i));
   }
   fprintf(out, "\n");
   printList(out, S);
   fprintf(out, "\n");

   freeList(&S);
   freeGraph(&G);
   freeGraph(&T);
   freeGraph(&C);
   return(0);
}


