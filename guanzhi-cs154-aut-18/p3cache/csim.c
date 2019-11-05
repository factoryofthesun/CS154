#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

int hits = 0, misses = 0, evics = 0; 

typedef struct {
	int valid, tag, counter;
	unsigned long long int address; 
} Set; 

void simulate(int s, int e, int b, char *filepath)
{
	int S = pow(2,s); 
//	int B = pow(2,b); 
	//Set up cache - 2d array of sets 
	Set **cache = (Set **) malloc(S * sizeof(Set *));
	for (int i = 0; i < S; i++)
	{
		cache[i] = (Set *) malloc(e * sizeof(Set)); 
		for (int j = 0; j < e; j++)
		{
			cache[i][j].valid = 0; 
			cache[i][j].counter = 0;
			cache[i][j].tag = 0; 
		}
	}	
	//Read tracefile
	FILE *fp = fopen(filepath, "r"); 
	char currentline[100]; 
	char access; 
	unsigned long long int addr; 
	int tagl = 64 - (s + b); 
	int found; 
	int count; 
	unsigned long long int tag; 
	unsigned long long int set;
	count = 0;  
	while(fgets(currentline, sizeof(currentline), fp) != NULL)
	{
		sscanf(currentline, " %c %llx,%*d", &access, &addr); 
		tag = addr >> (s + b); 
	    set = ((addr << tagl) >> (tagl + b)); 			
		found = 0;
		if (access == 'I')
		{ continue; }
		for (int i = 0; i < e; i++)
		{
			if ((cache[set][i].valid != 0) && (cache[set][i].tag == tag))
			{
				if (access == 'M')
				{ hits++;}
				hits++; 
				found = 1;
				count++;
				cache[set][i].counter = count;
				break; 
			} 
		}
		if(found == 0)
		{
			if (access == 'M')
			{ hits++;}
			misses++; 
			//Search for empty lines
			for (int i = 0; i < e; i++)
			{
				if (cache[set][i].valid == 0)
				{
					cache[set][i].tag = tag; 
					cache[set][i].valid = 1; 
					cache[set][i].counter = count; 
					found = 1; 
					break;
				}
			}
			//Search for LRU
			if (found == 0)
			{
				evics++; 
				int mini; 
				int mincount = count; 
				for (int i = 0; i < e; i++)
				{
					if (cache[set][i].counter < mincount)
					{
						mini = i; 
						mincount = cache[set][i].counter; 
					}
				}
				cache[set][mini].tag = tag; 
				cache[set][mini].counter = count;
			}
		}			
	}	

	//Free cache memory 
	for (int i = 0; i < S; i++)
	{
		free(cache[i]); 
	}		
	free(cache); 
}

int main(int argc, char *argv[])
{
	extern char *optarg; 
	extern int optind; 
	int sbits, E, bbits;
	char *path;
	int c = 0;  
	int temp = 0; 
    //Read in arguments
	while ((c = getopt(argc, argv, "s:E:b:t:")) != -1)
	{
		switch(c)
			{
				case 's':
					sscanf(optarg, "%d", &temp);
					sbits = temp; 
					break;
				case 'E':
					sscanf(optarg, "%d", &temp);	
					E = temp; 
					break;
				case 'b':
					sscanf(optarg, "%d", &temp);
					bbits = temp;
					break; 
				case 't':
					path = optarg; 
					break;
				case '?':
					fprintf(stderr, "Parameter not recognized\n"); 
					break; 
			}
	}					
	/*
    for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i],  "-s"))
		{
			sscanf(argv[i+1], "%d", &sbits);
		} 
		if (strcmp(argv[i], "-E"))
		{
			scanf(argv[i+1], "%d", &E);
		}
		if (strcmp(argv[i], "-b"))
		{
			sscanf(argv[i+1], "%d", &bbits); 
		} 
		if (strcmp(argv[i], "-t"))
		{
			path = argv[i+1]; 
		} 
	}
*/ 
	simulate(sbits,E,bbits,path); 
    printSummary(hits, misses, evics);
    return 0;
}
