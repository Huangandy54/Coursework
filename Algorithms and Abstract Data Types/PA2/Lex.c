/*
Andy Huang
ahuang44
PA2
Lex.c
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"List.h"
#define MAX_LEN 200

int main(int argc, char* argv[])
{
	if(argc!=3){
		printf("Usage: Lex <input file> <output file>\n");
		exit(1);
  	}
  	FILE *in,*out;
  	in = fopen(argv[1], "r");
  	char line[MAX_LEN];
  	out = fopen(argv[2], "w");
  	//counts lines in input file
  	int nLines=0;
  	while(fgets(line, MAX_LEN, in) != NULL) {
        nLines++;
    }
    //rewind goes back to front of file
    rewind(in);
    char words[nLines][MAX_LEN];
    for (int i = 0; i < nLines; i++)
    {
    	fgets(line,MAX_LEN,in);
    	strcpy(words[i],line);
    }
    //test print words array
    // for (int i = 0; i < nLines; ++i)
    // {
    // 	fprintf(stderr, "%s",words[i]);
    // }
    //start sorting exactly like pa1
    List L=newList();
    append(L,0);
    for (int x = 1; x < nLines; x++)
    {
    	moveFront(L);
    	while(strcmp(words[get(L)],words[x])<=0){
    		moveNext(L);

    		if (index(L)==-1)
    		{
    			break;
    		}
    	}
    	if (index(L)!=-1)
    	{
    		insertBefore(L,x);
    	}else{
    		append(L,x);
    	}
    }

    //move front and print whole list in order to output file
    moveFront(L);
    for (int i = 0; i < nLines; i++)
    {
    	fprintf(out, "%s", words[get(L)]);
    	moveNext(L);
    }
    fclose(out);
    fclose(in);
    freeList(&L);
	return 0;
}