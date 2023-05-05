#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"List.h"
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

  	fclose(in);
  	fclose(out);

}