/*
Andy Huang
ahuang44
PA4
FindPath.c 
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"Graph.h"
#define MAX_LEN 200

int main(int argc, char* argv[])
{
	if(argc!=3){
		printf("Usage: Lex <input file> <output file>\n");
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
  		addEdge(G,x,y);
  	}
  	//Print graph
  	printGraph(out,G);
  	//processing bfs and path queries
  	int source,destination;
  	List path=newList();
  	while(fscanf(in,"%d %d",&source,&destination)==2){
  		if (source==0||destination==0)
  		{
  			break;
  		}
  		BFS(G,source);
  		getPath(path,G,destination);
  		//not reachable
		if (getDist(G, destination) == INF){
			fprintf(out, "The distance from %d to %d is infinity\n", source, destination);
			fprintf(out, "No %d-%d path exists\n", source, destination);
			fprintf(out, "\n");
		}
		else{
			fprintf(out, "The distance from %d to %d is %d\n", source, destination, getDist(G,destination));
			fprintf(out, "A shortest %d-%d path is: ", source, destination);
			printList(out, path);
			fprintf(out, "\n\n");
		}
		clear(path);
  	}

  	freeList(&path);
  	freeGraph(&G);
  	fclose(in);
  	fclose(out);

}