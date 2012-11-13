/*adaptive_search.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "adaptive_search.h"




/*
adaptiveSearch
return n:number of wrong pieces left
*/
int adaptiveSearch(puzzle p, int **cur_sol)
{

	int* wrongs; //array of wrong pieces
	wrongs = (int *) malloc(p.n*sizeof(int));
	int n, found=1, new_score, score, i, j, tmp, rot, max, max_rot, old, index, jndex;


	//compute wrong pieces' array and actual score
	score=computeScore(p, cur_sol, wrongs, &n);
	if (n==0){
		//previous heuristics has already found optimum.
		return(0);
	}

	while(found){
		found=0;
		#ifndef WRONGS
		n=p.n;
		#endif

		for(i=0; i<n && !found;i++){

		    /*try rotating the piece in place */
		    index=i;
		    #ifdef WRONGS
            index=wrongs[i];
            #endif
		    max_rot=-1;
		    old=max=computeLocalScore(p, cur_sol, cur_sol[index][0], index, cur_sol[index][1]);
		    for(rot=0;rot<4;rot++){
                tmp=computeLocalScore(p, cur_sol, cur_sol[index][0], index, rot);
                if(tmp>max){
                    max=tmp;
                    max_rot=rot;
                }
            }
            if(max_rot>=0){
                cur_sol[index][1]=max_rot;
                score+=max-old;
                //TODO: update wrongs
                found=1;
            }
			for(j=i+1;j<n && !found;j++){
                    jndex=j;
                    #ifdef WRONGS
                    jndex=wrongs[j];
                    #endif


					//swap one to one between 2 wrong pieces
					new_score=trySwapPieces(p, cur_sol, score, index, jndex, wrongs,&n );

					if(new_score>score){
						found=1;
						score=new_score;
					}

					//else try other 2 pieces
			}
		}
	}

    free(wrongs);
	return(0);



}

/*computeWrongs
* p: puzzle
* wrongs: array of int, where each element is position of a wrong piece
* returns: total number of wrong elements */
/*int computeWrongs(puzzle *p, int* wrongs){
	//search for wrong pieces

	//alloc wrongs
}*/

/*trySwapPieces
* compute the score the puzzle WOULD have, if swap occured
* p:puzzle
* i, j: pieces to be swap
* returns: new total score with the swap
*
*/
int trySwapPieces(puzzle p,int **cur_sol,int score, int i, int j, int *wrongs, int *wsize_ptr){
	//newscores=oldscores-computeLocalScore(....,cur_sol, i)-computeLocalScore(.....,cur_sol, j)
	//.... +computeLocalScore(...., swapped_sol, i)+computeLocalScore(....,swapped_sol, j);


	int rot, tmp, old_i, old_j, old_rot_i, old_rot_j, k, maxi=0, maxj=0, max_rot_i, max_rot_j, found, w, skip=0;
	int scoreold=score;

	//store actual pieces and rotations:
	old_i=cur_sol[i][0];
	old_j=cur_sol[j][0];
	old_rot_i=cur_sol[i][1];
	old_rot_j=cur_sol[j][1];

	score-=computeLocalScore(p, cur_sol, cur_sol[i][0], i, cur_sol[i][1]);
	score-=computeLocalScore(p, cur_sol, cur_sol[j][0], j, cur_sol[j][1]);
	if(isNeighbor(p, i, j)){


        /* try rotating the two pieces in place */

        score+=checkEdge(p, cur_sol, i, j); // score for common edge was removed twice, so re-add once
        for(cur_sol[i][1]=0;cur_sol[i][1]<4;cur_sol[i][1]++){ //rotate i

			for(cur_sol[j][1]=0;cur_sol[j][1]<4;cur_sol[j][1]++){ //rotate j
			    k=computeLocalScore(p, cur_sol, old_i, i, cur_sol[i][1]); //score from i
				tmp=computeLocalScore(p, cur_sol, old_j, j, cur_sol[j][1]); //score from j
                if(tmp+k>maxi+maxj){
					maxi=k;
					maxj=tmp;
					max_rot_i=cur_sol[i][1];
					max_rot_j=cur_sol[j][1];
                }
			}
        }
        score+=maxi+maxj; // this adds one point for each "good" edge, so the common edge has double score
        //update solution
        cur_sol[i][1]=max_rot_i;
        cur_sol[j][1]=max_rot_j;
        score-=checkEdge(p, cur_sol, i, j); //score for common edge has been added twice, then subtract once
        if (score>scoreold){

            skip=1; //-> don't try swapping; maybe next time...
        } else {
            //restore
            cur_sol[i][1]=old_rot_i;
            cur_sol[j][1]=old_rot_j;
            score-=maxi+maxj;
        }


        if(!skip){
            /* swap the pieces (if leads to better score) */

            cur_sol[i][0]=old_j; //move j-->i
            cur_sol[j][0]=old_i; //move i-->j
            for(cur_sol[i][1]=0;cur_sol[i][1]<4;cur_sol[i][1]++){ //rotate i

                for(cur_sol[j][1]=0;cur_sol[j][1]<4;cur_sol[j][1]++){ //rotate j
                    k=computeLocalScore(p, cur_sol, old_j, i, cur_sol[i][1]); //score from i
                    tmp=computeLocalScore(p, cur_sol, old_i, j, cur_sol[j][1]); //score from j
                    if(tmp+k>maxi+maxj){
                        maxi=k;
                        maxj=tmp;
                        max_rot_i=cur_sol[i][1];
                        max_rot_j=cur_sol[j][1];
                    }
                }
            }
        score+=maxi+maxj; // this adds one point for each "good" edge, so the common edge has double score
        score-=checkEdge(p, cur_sol, i, j); //score for common edge has been added twice, then subtract once
            if (score>scoreold){
                //update solution
                cur_sol[i][1]=max_rot_i;
                cur_sol[j][1]=max_rot_j;
            } else {
                //restore old values:
                cur_sol[i][0]=old_i;
                cur_sol[i][1]=old_rot_i;
                cur_sol[j][0]=old_j;
                cur_sol[j][1]=old_rot_j;
                score=scoreold;
            }
        }


	} else {
		for(rot=0;rot<4;rot++){
			tmp=computeLocalScore(p, cur_sol, old_i, j, rot);
			if(tmp>maxi){
				maxi=tmp;
				max_rot_i=rot;
			}
		}
        score+=maxi;


		maxj=0;
		for(rot=0;rot<4;rot++){
			tmp=computeLocalScore(p, cur_sol, old_j, i, rot);
			if(tmp>maxj){
				maxj=tmp;
				max_rot_j=rot;
			}
		}
		score+=maxj;
		if(score>scoreold){
            cur_sol[j][0]=old_i;
            cur_sol[j][1]=max_rot_i;
            cur_sol[i][0]=old_j;
            cur_sol[i][1]=max_rot_j;
		}else{
            return(scoreold);
		}
	}

	/* update wrongs vector */
	#ifdef WRONGS
	if(maxi==MAX_LOCAL_SCORE){
		removeWrong(p, cur_sol, wrongs, wsize_ptr, i);
	}
	// update neighbors of i
	updateWrongs(p, cur_sol, wrongs, wsize_ptr, i);

	if(maxj==MAX_LOCAL_SCORE){
		removeWrong(p, cur_sol, wrongs, wsize_ptr, i);
	}
	// update neighbors of j
	updateWrongs(p, cur_sol, wrongs, wsize_ptr, i);
    #endif

	return(score);
}

void updateWrongs(puzzle p, int **cur_sol, int *wrongs, int *wsize_ptr, int i)
{
    int off_i[4]={1, -1, -p.w_size, p.w_size};
    int k, ls, j;
    for(k=0; k<4; k++){ // for each neighbor(i):
        j=i+off_i[k];
        if(j>=0 && j<p.n){
            ls = computeLocalScore(p, cur_sol, cur_sol[j][0],j, cur_sol[j][1]);
            if(ls==MAX_LOCAL_SCORE){
                removeWrong(p, cur_sol, wrongs, wsize_ptr, j);
            } else {
                addWrong(p, cur_sol, wrongs, wsize_ptr, j);
            }
        }

    }

    return;

}

void addWrong(puzzle p, int **cur_sol, int *wrongs, int *wsize_ptr, int i)
{
    int w;
    for(w=0;w < *wsize_ptr;w++){
			if(wrongs[w]==i)return; // check if i is already present
    }
    wrongs[*wsize_ptr]=i; // add i to the bottom of the array
    *wsize_ptr++;
    return;
}

void removeWrong(puzzle p, int **cur_sol, int *wrongs, int *wsize_ptr, int i)
{
    int found=0, w;
		for(w=0;w < *wsize_ptr;w++){
			if(wrongs[w]==i)found=1;
			if(found && w<(*wsize_ptr)-1) wrongs[w]=wrongs[w+1];
		}
		*wsize_ptr--;
		return;
}

/* DEPRECATED */
/*
//int swapPieces(puzzle *p,int cur_sol[][2], int i, int j, int **wrong, int *wsize_ptr){
int swapPieces(puzzle p,int** cur_sol, int i, int j, int *wrongs, int *wsize_ptr){


	int max_rot, max_rot_i, max_rot_j, rot, maxi=0, maxj=0, score,old_i, old_j, found, w, tmp, k;

	old_i=cur_sol[i][0];
	old_j=cur_sol[j][0];

	if(isNeighbor(p, i, j)){
		cur_sol[i][0]=old_j; //move j-->i
		cur_sol[j][0]=old_i; //move i-->j
		for(cur_sol[i][1]=0;cur_sol[i][1]<4;cur_sol[i][1]++){ //rotate i

			for(cur_sol[j][1]=0;cur_sol[j][1]<4;cur_sol[j][1]++){
			    k=computeLocalScore(p, cur_sol, old_j, i, cur_sol[i][1]); //score from i
				tmp=computeLocalScore(p, cur_sol, old_i, j, cur_sol[j][1]); //score from j
				if(tmp+k>maxi+maxj){
					maxi=k;
					maxj=tmp;
					max_rot_i=cur_sol[i][1];
					max_rot_j=cur_sol[j][1];
				}
			}
		}
		cur_sol[j][0]=old_i;
		cur_sol[i][1]=max_rot_i;
		cur_sol[j][1]=max_rot_j;
		score+=maxi+maxj;

	} else {
		for(rot=0;rot<4;rot++){
			tmp=computeLocalScore(p, cur_sol, old_i, j, rot);
			if(tmp>maxi){
				maxi=tmp;
				max_rot=rot;
			}
		}

		cur_sol[j][0]=old_i;
		cur_sol[j][1]=max_rot;

		maxj=0;
		for(rot=0;rot<4;rot++){
			tmp=computeLocalScore(p, cur_sol, old_j, i, rot);
			if(tmp>maxj){
				maxj=tmp;
				max_rot=rot;
			}
		}

		cur_sol[i][0]=old_j;
		cur_sol[i][1]=max_rot;
	}


	if(maxi==MAX_LOCAL_SCORE){
		//update wrongs;
		found=0;
		for(w=0;w < *wsize_ptr;w++){
			if(wrongs[w]==j)found=1;
			if(found && w<(*wsize_ptr)-1) wrongs[w]=wrongs[w+1];
		}
		*wsize_ptr=*wsize_ptr-1;
	}

	if(maxj==MAX_LOCAL_SCORE){
		//update wrongs;
		found=0;
		for(w=0;w < *wsize_ptr;w++){
			if(wrongs[w]==i)found=1;
			if(found && w<(*wsize_ptr)-1) wrongs[w]=wrongs[w+1];
		}
		*wsize_ptr=*wsize_ptr-1;
	}

	return(0);



}*/

int isNeighbor(puzzle p, int i, int j){
	if( i==j-1 || i==j+1 || i==j-p.w_size || i==j+p.w_size) return(1);
	return(0);
}










/*

1) Problema dei bordi   ---> RISOLTO MA CODICE NON PIU' PULITO COME PRIMA
   Aggiunta funzione isBorder  per fare la trySwapPieces sensibile ai bordi

2) Problema dei vicini
   Copiare la curSol in una struttura temporanea e effettivamente scambiare le posizioni dei due pezzi e con algoritmo greedy si stabilisce la rotazione e il punteggio massimo corretto

   POSSIBILE SOLUZIONE:

   per tutte le rotazioni del pezzo x
       per tutte le rotazioni del pezzo y
            calcolo punteggio x
            calcolo punteggio y
            new_score=somma dei due punteggi facendo attenzione a contare una sola volta i colori vicini




int isBorder(puzzle p, int **cur_sol, int piece, int pos, int rot) {
    int total=0;
    int **col=p.colours;
    //OPTIMIZATION//
    //rimpiazza i nomi direttamente con i valori...leggibilità vs memoria?

    //mappa per i colori dopo la rotazione
    int colS=(0+3*rot)%4;
    int colW=(1+3*rot)%4;
    int colN=(2+3*rot)%4;
    int colE=(3+3*rot)%4;

    //mappa per i pezzi a nord sud est ovest da tenere in conto
    int N=pos-p.w_size;
    int S=pos+p.w_size;
    int E=pos+1;
    int W=pos-1;

    if ((N<0) && (col[piece][colN]==0))          //North side is a border
        total++;


    if (((E%p.w_size)==0) && (col[piece][colE]==0))      //Est side is a border
        total++;


    if ((((W%p.w_size)==(p.w_size-1)) || (W%p.w_size)==-1) && (col[piece][colW]==0))      //West side is a border
        total++;


    if ((S>=p.n) && (col[piece][colS]==0))            //South side is a border
        total++;

    return(total);
}





int trySwapPieces(puzzle p,int **cur_sol,int score, int i, int j){
	//newscores=oldscores-computeLocalScore(....,cur_sol, i)-computeLocalScore(.....,cur_sol, j)
	//.... +computeLocalScore(...., swapped_sol, i)+computeLocalScore(....,swapped_sol, j);
	int rot, final_rot, tmp, max, border;

    score-=2*computeLocalScore(p, cur_sol, cur_sol[i][0], i, cur_sol[i][1]);
	border=isBorder(p, cur_sol, cur_sol[i][0], i, cur_sol[i][1]);
	if (border==1)
        score+=1;
    else if(border==2)
        score+=2;

	//...

	score-=2*computeLocalScore(p, cur_sol, cur_sol[j][0], j, cur_sol[j][1]);
    border=isBorder(p, cur_sol, cur_sol[j][0], j, cur_sol[j][1]);
    if (border==1)
        score+=1;
    else if(border==2)
        score+=2;





	max=0;
	for(rot=0;rot<4;rot++){
		tmp=computeLocalScore(p, cur_sol, cur_sol[i][0], j, rot);
		if (tmp>max){
            max=tmp;
		    final_rot=rot;
        }
	}
	score+=2*max;
	border=isBorder(p, cur_sol, cur_sol[i][0], j, final_rot);
    if (border==1)
        score-=1;
    else if(border==2)
        score-=2;


	max=0;
	for(rot=0;rot<4;rot++){
		tmp=computeLocalScore(p, cur_sol, cur_sol[j][0], i, rot);
		if(tmp>max){ max=tmp; final_rot=rot;}
	}
	score+=2*max;
	border=isBorder(p, cur_sol, cur_sol[j][0], i, final_rot);
    if (border==1)
        score-=1;
    else if(border==2)
        score-=2;


	return(score);
}




*/


