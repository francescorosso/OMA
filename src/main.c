#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "greedy_rand.h"
#include "adaptive_search.h"

#define LEN 32

int basket_size=3;
int pieces_nr=3;

int main(int argc, char* argv[]) {

    int i, score;
    char fileinput[LEN], fileoutput[LEN];

    if(argc < 3){
        printf("Syntax: %s [-k <num>] [-p <nump>] <file_input> <file_output>", argv[0]);
        return(0);
    }
    i=1;
    while(i<argc-2){
        switch(argv[i][1]){
            case 'k':
                basket_size = atoi(argv[i+1]);
                i+=2;
                break;
            case 'p':
                pieces_nr = atoi(argv[1]);
                i+=2;
                break;
            default: i++;

        }
    }
    strcpy(fileinput,argv[argc-2]);
    strcpy(fileoutput,argv[argc-1]);
//PART 1 - READ INPUT FILE
	//open input file
    puzzle p;

	FILE *fin = fopen(fileinput, "r");

	//alloc memory for puzzle struct

	//read puzzle dimension
	fscanf(fin, "%d %d", &(p.w_size), &(p.h_size));
//	printf("%d %d\n", p->w_size, p->h_size);
	p.n = p.w_size * p.h_size;




	//alloc memory for pieces array
	int **colors=(int**)malloc(sizeof(int*)*p.n);
        for(i=0;i<p.n;i++)
            colors[i]=(int*)malloc(sizeof(int)*4);
    p.colours=colors;

	//read pieces and store them into an array (colors)
	for (i = 0; i < p.n; i++) {
		fscanf(fin, "%d %d %d %d", &colors[i][0], &colors[i][1], &colors[i][2], &colors[i][3]);
	}
//	for(i = 0; i < n; i++) {
//	printf("%d %d %d %d\n", colors[i][0], colors[i][1], colors[i][2], colors[i][3]);
//	}

	fclose(fin);

//PART 2 - GREEDY RANDOMIZED

    int **cur_sol=(int**)malloc(p.n*sizeof(int*));
        for(i=0;i<p.n;i++)
            cur_sol[i]=(int*)malloc(2*sizeof(int));
	//int final_sol[2][p.n];
	//int cur_score, final_score;



	#ifdef MSANGLE
	multiStartAnglePermutation(p,cur_sol);
	#endif

	#ifdef MSANGPLO
	multiStartAnglePermutationPlusOne(p,cursol);
	#endif

	#ifdef MSANGPLP
	multiStartAnglePermutationPlusP(p,cur_sol, pieces_nr);
	#endif

	#ifndef MSANGPLP
        #ifndef MSANGPLO
            #ifndef MSANGLE
                greedyRandomizedSpiral(p,cur_sol);      //GREEDY
                adaptiveSearch(p,cur_sol);              //PART 2.1 ADAPTIVE SEARCH
            #endif
        #endif
	#endif


	score=computeScore(p,cur_sol,NULL,NULL);


//PART 3 - PRINT OUTPUT FILE (and go home)
	FILE *fout = fopen(fileoutput, "w");
	for (i = 0; i < p.n; i++) {
		fprintf(fout, "%d %d\n", cur_sol[i][0], cur_sol[i][1]);
	}
    fclose(fout);


	return(score);
}








