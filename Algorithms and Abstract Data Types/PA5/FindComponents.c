/*
Andy Huang
ahuang44
PA5
FindComponents.c 
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"Graph.h"
#define MAX_LEN 200

int main(int argc, char* argv[])
{
  if(argc!=3){
    printf("Usage: FindComponents <input file> <output file>\n");
    exit(1);
    }
    FILE *in,*out;
    in = fopen(argv[1], "r");
    //char line[MAX_LEN];
    out = fopen(argv[2], "w");
    //process graph
    int numVerts,x,y;
    fscanf(in,"%d",&numVerts);
    Graph G = newGraph(numVerts);
    while(fscanf(in,"%d %d",&x,&y)==2){
      if (x==0||y==0)
      {
        break;
      }
      addArc(G,x,y);
    }
    //Print graph
    fprintf(out, "Adjacency list representation of G:\n");
    printGraph(out,G);
    fprintf(out, "\n");
    //Setup List
    
    List S=newList();
    for (int i = 1; i <= numVerts; i++)
    {
        append(S,i);
    }
    DFS(G,S);
    //run dfs again but on transpose and in decreasing finish time
    Graph T=transpose(G);
    DFS(T,S);
    //count components
    int scc=0;
    for (int i = 1; i <= getOrder(T); i++)
    {
        if (getParent(T,i)==NIL)
        {
            scc++;
        }
    }
    fprintf(out, "G contains %d strongly connected components:\n", scc);
    //print SCCs
    List temp=newList();
    int sccCounter=1;
    //start at back of List and go to front. add to a temp list. print and clear temp list when a NIL is found meaning end of a SCC.
    moveBack(S);
    while(index(S) != -1){
        int n = get(S);
        prepend(temp,n);
            if(getParent(T,n) == NIL)
            {
                fprintf(out,"Component %d:",sccCounter);
                printList(out,temp);
                fprintf(out,"\n");
                clear(temp);
                sccCounter++;
            }
        movePrev(S); 
    }


    freeList(&temp);
    freeList(&S);
    freeGraph(&G);
    freeGraph(&T);
    fclose(in);
    fclose(out);

}