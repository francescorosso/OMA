#include <time.h>
#include <stdlib.h>
#include "greedy_rand.h"


int haveBorderColour(puzzle p, int piece);
int isBorder(puzzle p,int **cur_provv,int pos);
int checkEdge(puzzle p, int **cur_sol,int i,int j);
void visit(int *value, int r, int **permutation);
int takeKBestScore (int *localscores,int *basket,int *available, int number_pice);
void choosePiece (puzzle p, int **cur_sol, int *basket, int *available, int *localscores, int pos);


int greedy_randomized (puzzle p, int **cur_sol, int *available) {

    int pos;

	//save local scores
	//0->4 if piece available
    int *localscores;
	localscores = (int *) malloc(p.n*sizeof(int));

	//save best choice, only with piece number
	#ifdef DYNBKT
    int basket[p.n];
    #else
	int basket[basket_size];
	#endif

    srand(time(NULL));  //pseudo-random number generator is initialized using the argument passed.

	for (pos=0;pos<p.n;pos++)      	  //For each position in the puzzle
        if (cur_sol[pos][0]==-1)
           choosePiece(p,cur_sol,basket,available,localscores,pos);

    free(localscores);

    return(computeScore(p,cur_sol,NULL,NULL));
}

int greedyRandomized (puzzle p, int **cur_sol) {

    //save pieces availability
	//0 -> not available
	//1 -> available
    int i,score,*available;
    available = (int *) malloc(p.n*sizeof(int));

	for (i=0 ; i<p.n ; i++) {
        available[i]=1;         //set vector to 1 [=available]
        cur_sol[i][0]=-1;        //set -1 in each position, useful in localscores
	}

    score=greedy_randomized(p,cur_sol,available);

    free(available);

    return(score);
}


int greedy_randomized_spiral (puzzle p, int **cur_sol, int *available) {

    int r,c,s,j,i,pos;

	//save local scores
	//0->4 if piece available
    int *localscores;
	localscores = (int *) malloc(p.n*sizeof(int));

	//save best choice, only with piece number
	#ifdef DYNBKT
    int basket[p.n];
    #else
	int basket[basket_size];
	#endif

    srand(time(NULL));  //pseudo-random number generator is initialized using the argument passed.

    c = p.w_size-1;
    r = p.h_size-1;
    s = 0;

    do {
        for (i=j=s;j<c;j++) {      //left->right
            pos=i*p.w_size+j;
            if (cur_sol[pos][0]==-1)
                choosePiece(p,cur_sol,basket,available,localscores,pos);
        }
        for (;i<r;i++) {           //up->down
            pos=i*p.w_size+j;
            if (cur_sol[pos][0]==-1)
                choosePiece(p,cur_sol,basket,available,localscores,pos);
        }
        for (;j>s;j--) {           //right->left
            pos=i*p.w_size+j;
            if (cur_sol[pos][0]==-1)
                choosePiece(p,cur_sol,basket,available,localscores,pos);
        }
        for (;i>s;i--) {           //down->up
            pos=i*p.w_size+j;
            if (cur_sol[pos][0]==-1)
                choosePiece(p,cur_sol,basket,available,localscores,pos);
        }
        r--;
    } while((++s)<(--c));

    if (s==c) {
       pos=c*p.w_size+s;
       if (cur_sol[pos][0]==-1)
          choosePiece(p,cur_sol,basket,available,localscores,pos);
    }

    free(localscores);

    return(computeScore(p,cur_sol,NULL,NULL));
}

int greedyRandomizedSpiral (puzzle p, int **cur_sol) {

    //save pieces availability
	//0 -> not available
	//1 -> available
    int i,score,*available;
    available = (int *) malloc(p.n*sizeof(int));

	for (i=0 ; i<p.n ; i++) {
        available[i]=1;         //set vector to 1 [=available]
        cur_sol[i][0]=-1;        //set -1 in each position, useful in localscores
	}
    score=greedy_randomized_spiral(p,cur_sol,available);

    free(available);

    return(score);
}


void choosePiece (puzzle p, int **cur_sol, int *basket, int *available, int *localscores, int pos) {

    int j,point,rot,took,rnd;

    for (j=0 ; j<p.n ; j++)             //for each pice
	        if (available[j]) {             //if is available
	        localscores[j]=0;                          //set local score to 0
                for (rot=0 ; rot<4 ; rot++) {                  //for each rotation compute the score
                    point=computeLocalScore(p, cur_sol, j, pos, rot);
                    if (point>localscores[j])
                        localscores[j]=point;                  //update score if better
                }
            }

    took = takeKBestScore(localscores,basket,available,p.n);                  //fill up the basket with K best score

    if (took>basket_size)
       rnd=rand()%basket_size;
    else
       rnd=rand()%took;

    available[basket[rnd]]=0;                                 //take one pice from the basket with pseudo-random algorithm
    cur_sol[pos][0]=basket[rnd];

    localscores[basket[rnd]]=0;

    for (rot=0 ; rot<4 ; rot++) {
        point=computeLocalScore(p, cur_sol, basket[rnd], pos, rot);
        if (point>localscores[basket[rnd]]){
           localscores[basket[rnd]]=point;
           cur_sol[pos][1]=rot;
        }
    }
}

int takeKBestScore (int *localscores,int *basket,int *available, int number_pice) {

    int i,j,count,took=0;

    #ifndef DYNBKT

    for(i=4 ; i>=0 && took<basket_size ; i--)
        for (j=0 ; j<number_pice && took<basket_size ; j++)
            if (available[j])
                if (localscores[j]==i) {
                    basket[took]=j;
                    took++;
                }

    #else

    count=took=0;
    for(i=4 ; i>=0 && count==0 ; i--)
        for (j=0 ; j<number_pice ; j++)
            if (available[j])
                if (localscores[j]==i) {
                    basket[took]=j;
                    took++;
                    count=1;
                }
    basket_size=took;

    #endif

    return(took);
}

int computeLocalScore (puzzle p, int **cur_sol, int piece, int pos, int rot) {
    int score;
    int **col=p.colours;

    //map of colour after rotation
    int colS=(0+3*rot)%4;
    int colW=(1+3*rot)%4;
    int colN=(2+3*rot)%4;
    int colE=(3+3*rot)%4;

    //which pos I have nord sud est ovest?
    int N=pos-p.w_size;
    int S=pos+p.w_size;
    int E=pos+1;
    int W=pos-1;

    score=0;

    if ((N<0) && (col[piece][colN]==0))          //North side is a border
               score++;
    else if ((N>=0) && (cur_sol[N][0]!=-1) && (col[piece][colN]==col[cur_sol[N][0]][(0+3*cur_sol[N][1])%4]))
            score++;

    if (((E%p.w_size)==0) && (col[piece][colE]==0))      //Est side is a border
            score++;
    else if (((E%p.w_size)!=0) && (cur_sol[E][0]!=-1) && (E<p.n) && (col[piece][colE])==(col[cur_sol[E][0]][(1+3*cur_sol[E][1])%4]))
            score++;

    if ((((W%p.w_size)==(p.w_size-1)) || (W%p.w_size)==-1) && (col[piece][colW]==0))      //West side is a border
            score++;
    else if ((((W%p.w_size)!=(p.w_size-1)) && ((W%p.w_size)!=-1)) && (W>=0)  && (cur_sol[W][0]!=-1) && ((col[piece][colW])==(col[cur_sol[W][0]][(3+3*cur_sol[W][1])%4])))
            score++;

    if ((S>=p.n) && (col[piece][colS]==0))            //South side is a border
            score++;
    else if ((S<p.n) && (col[piece][colS]!=0) && (S<p.n) && (cur_sol[S][0]!=-1) && (col[piece][colS]==col[cur_sol[S][0]][(2+3*cur_sol[S][1])%4]))
            score++;

    return(score);
}

int computeScore (puzzle p, int **cur_sol, int *wrongs, int *wsize_ptr) {
    int i,j,total,score;

    if (wrongs!=NULL && wsize_ptr!=NULL) {              //function
        *wsize_ptr=0;
        for (i=0 , total=0 ,j=0 , score=0; i<p.n ; i++) {
            score=computeLocalScore(p,cur_sol,cur_sol[i][0], i,cur_sol[i][1]);
            if (score<MAX_LOCAL_SCORE) {
                wrongs[j]=i;
                j++;
            }
            total+=score;
        }
        *wsize_ptr=j;
    }
    else {
        for (i=0 , score=0; i<p.n ; i++)
            score+=computeLocalScore(p,cur_sol,cur_sol[i][0], i,cur_sol[i][1]);
        total=score;
    }
    return(total);
}


void visit(int *value, int r, int **permutation) {

  int i,j;
  static int level = -1;
  static int sol = -1;

  level = level+1;
  value[r] = level;

  if (level == 3)
    for (j=0, sol++; j<3; j++)
        permutation[sol][j]=value[j];     //Save current permutation
  else
    for (i = 0; i < 3; i++)
      if (value[i] == 0)
        visit(value, i, permutation);

  level = level-1;
  value[r] = 0;
  if (sol==5 && level==-1)        //Alla fine della recursione, riporto la variabile statica al suo valore originale per poter richiamare correttamente la funzione
      sol=-1;
}


int multiStartAnglePermutation(puzzle p, int **cur_sol) {

    int angle[4];
    int pos[4];
    int i,j,c,count,rot,max,score,m,n;

    int **col=p.colours;

    int **cur_provv=(int**)malloc(p.n*sizeof(int*));
    for (i=0;i<p.n;i++)
        cur_provv[i]=(int*)malloc(2*sizeof(int));

    int **permutation=(int**)malloc(6*sizeof(int*));       //Conterrà tutte le permutazioni dei numeri 0 1 2 3
    for (i=0;i<6;i++)
        permutation[i]=(int*)malloc(4*sizeof(int));

    int *available;
    available = (int *) malloc(p.n*sizeof(int));


    pos[0]=0;                       //posizione dei 4 angoli
    pos[1]=p.w_size-1;
    pos[2]=p.n-p.w_size;
    pos[3]=p.n-1;

    for (i=0;i<4;i++){
        angle[i]=-1;
    }

    for (i=count=c=0;i<p.n; i++ , c=0) {          //Trova gli angoli
        for (j=0;j<4;j++)
            if (col[i][j]==0)
                c++;
        if (c==2) {
            angle[count]=i;
            count++;
        }
    }


    //Alexander Bogomolyn's unordered permutation algorithm
    //START

    int value[4];
    for (j = 0; j < 4; j++)
         value[j] = 0;

    visit(value, 0, permutation);
    for (i=0;i<6;i++)
        permutation[i][3]=0;
    //END



    for (i=max=0;i<6;i++){         //For each permutaion

        for (c=0 ; c<p.n ; c++) {
            cur_provv[c][0]=-1;        //set -1 in each position of cur_provv, useful in localscores
            available[c]=1;
        }

        for (j=0;j<4; j++)                   //Set correct rotation for each angle
            for (rot=0;rot<4;rot++)
                if ((count=computeLocalScore (p,cur_provv,angle[permutation[i][j]], pos[j],rot))==2) {
                    cur_provv[pos[j]][1]=rot;
                    break;
                }

        for (j=0; j<4; j++ ) {
            cur_provv[pos[j]][0]=angle[permutation[i][j]];        //metti i pezzi in posiz angolare
            available[angle[permutation[i][j]]]=0;
        }


        greedy_randomized_spiral(p,cur_provv,available);
        adaptiveSearch(p,cur_provv);

        if ((score=computeScore(p,cur_provv,NULL,NULL))>max){      //Fai partire il l'algoritmo e salva il punteggio per trovare poi il migliore.
            for(m=0;m<p.n;m++)
                for(n=0;n<2;n++ )
                    cur_sol[m][n]=cur_provv[m][n];
            max=score;
        }
    }

    free(available);
    for (i=0;i<6;i++)
        free(permutation[i]);
    free(permutation);
    for (i=0;i<p.n;i++)
        free(cur_provv[i]);
    free(cur_provv);

    return(max);
}

int multiStartAnglePermutationPlusOne (puzzle p, int **cur_sol) {

    int angle[4];
    int pos[4];
    int i,j,c,r,count,rot,max,score,piece,m,n;

    int **col=p.colours;

    int **cur_provv=(int**)malloc(p.n*sizeof(int*));
    for (i=0;i<p.n;i++)
        cur_provv[i]=(int*)malloc(2*sizeof(int));

    int **permutation=(int**)malloc(6*sizeof(int*));       //Conterrà tutte le permutazioni dei numeri 0 1 2 3
    for (i=0;i<6;i++)
        permutation[i]=(int*)malloc(4*sizeof(int));

    int *available;
    available = (int *) malloc(p.n*sizeof(int));


    pos[0]=0;                       //posizione dei 4 angoli
    pos[1]=p.w_size-1;
    pos[2]=p.n-p.w_size;
    pos[3]=p.n-1;

    for (i=0;i<4;i++){
        angle[i]=-1;
    }

    for (i=count=c=0;i<p.n; i++ , c=0) {          //Trova gli angoli
        for (j=0;j<4;j++)
            if (col[i][j]==0)
                c++;
        if (c==2) {
            angle[count]=i;
            count++;
        }
    }


    //Alexander Bogomolyn's unordered permutation algorithm
    //START

    int value[4];
    for (j = 0; j < 4; j++)
         value[j] = 0;

    visit(value, 0, permutation);
    for (i=0;i<6;i++)
        permutation[i][3]=0;
    //END

    int border[((p.w_size-2)*4)];
    for (i=j=0;i<p.n;i++)
        if (haveBorderColour(p,i)==1) {
            border[j]=i;
            j++;
        }



    for (i=max=0;i<6;i++)
        for ( r = 0; r<(p.w_size-2)*4 ;r++) {         //For each permutaion and each border piece

            for (c=0 ; c<p.n ; c++) {
                cur_provv[c][0]=-1;        //set -1 in each position of cur_provv, useful in localscores
                available[c]=1;
            }

            for (j=0;j<4; j++)                   //Set correct rotation for each angle
                for (rot=0;rot<4;rot++)
                    if ((count=computeLocalScore (p,cur_provv,angle[permutation[i][j]], pos[j],rot))==2) {
                        cur_provv[pos[j]][1]=rot;
                    break;
                }

            for (j=0; j<4; j++ ) {
                cur_provv[pos[j]][0]=angle[permutation[i][j]];        //metti i pezzi in posiz angolare
                available[angle[permutation[i][j]]]=0;
            }

            piece= border[r];

            for (rot=0;rot<4;rot++){
                if (col[piece][(2+3*rot)%4]==0)
                    break;
                }
            cur_provv[2][0]=piece;
            cur_provv[2][1]=rot;
            available[piece]=0;

            greedy_randomized_spiral(p,cur_provv,available);
            adaptiveSearch(p,cur_provv);

            if ((score=computeScore(p,cur_provv,NULL,NULL))>max){      //Fai partire il l'algoritmo e salva il punteggio per trovare poi il migliore.
                for(m=0;m<p.n;m++)
                    for(n=0;n<2;n++ )
                        cur_sol[m][n]=cur_provv[m][n];
                max=score;
            }
        }

    free(available);
    for (i=0;i<6;i++)
        free(permutation[i]);
    free(permutation);
    for (i=0;i<p.n;i++)
        free(cur_provv[i]);
    free(cur_provv);

    return(max);
}

int multiStartAnglePermutationPlusP (puzzle p, int **cur_sol, int n_pice) {

    if (n_pice>((p.w_size-2)*4))
        return (-1);

    int angle[4];
    int pos[4];
    int i,j,c,z,count,rot,max,score,piece,position,m,n;

    int **col=p.colours;

    int **cur_provv=(int**)malloc(p.n*sizeof(int*));
    for (i=0;i<p.n;i++)
        cur_provv[i]=(int*)malloc(2*sizeof(int));

    int **permutation=(int**)malloc(6*sizeof(int*));       //Conterrà tutte le permutazioni dei numeri 0 1 2 3
    for (i=0;i<6;i++)
        permutation[i]=(int*)malloc(4*sizeof(int));

    int *available;
    available = (int *) malloc(p.n*sizeof(int));


    pos[0]=0;                       //posizione dei 4 angoli
    pos[1]=p.w_size-1;
    pos[2]=p.n-p.w_size;
    pos[3]=p.n-1;

    for (i=0;i<4;i++){
        angle[i]=-1;
    }

    for (i=count=c=0;i<p.n; i++ , c=0) {          //Trova gli angoli
        for (j=0;j<4;j++)
            if (col[i][j]==0)
                c++;
        if (c==2) {
            angle[count]=i;
            count++;
        }
    }


    //Alexander Bogomolyn's unordered permutation algorithm
    //START

    int value[4];
    for (j = 0; j < 4; j++)
         value[j] = 0;

    visit(value, 0, permutation);
    for (i=0;i<6;i++)
        permutation[i][3]=0;
    //END

    int border[((p.w_size-2)*4)];
    for (i=j=0;i<p.n;i++)
        if (haveBorderColour(p,i)==1) {
            border[j]=i;
            j++;
        }



    for (i=max=0;i<6;i++) {         //For each permutaion

            for (c=0 ; c<p.n ; c++) {
                cur_provv[c][0]=-1;        //set -1 in each position of cur_provv, useful in localscores
                available[c]=1;
            }

            for (j=0;j<4; j++)                   //Set correct rotation for each angle
                for (rot=0;rot<4;rot++)
                    if ((count=computeLocalScore (p,cur_provv,angle[permutation[i][j]], pos[j],rot))==2) {
                        cur_provv[pos[j]][1]=rot;
                    break;
                }

            for (j=0; j<4; j++ ) {
                cur_provv[pos[j]][0]=angle[permutation[i][j]];        //metti i pezzi in posiz angolare
                available[angle[permutation[i][j]]]=0;
            }

            for (z=position=piece=0;z<n_pice; z++, position=piece=0) {

                while (piece==0) {              //prendo un pezzo available
                    piece=border[rand()%((p.w_size-2)*4)];
                    if (available[piece]==1)
                        break;
                    piece=0;
                }

                 while (position==0) {              //prendo una posizione bordo
                    position=rand()%p.n;
                    if (isBorder(p,cur_provv,position)==1 && cur_provv[position][0]==-1)      //Devo prendere random una posizione bordo non occupata già
                            break;
                    position=0;
                }

                for (rot=0;rot<4;rot++)
                    if (computeLocalScore (p,cur_provv,piece, position,rot)==1) {
                        cur_provv[position][1]=rot;
                        break;
                    }

                cur_provv[position][0]=piece;
                available[piece]=0;
            }


            greedy_randomized_spiral(p,cur_provv,available);
            adaptiveSearch(p,cur_provv);

            if ((score=computeScore(p,cur_provv,NULL,NULL))>max){      //Fai partire il l'algoritmo e salva il punteggio per trovare poi il migliore.
                for(m=0;m<p.n;m++)
                    for(n=0;n<2;n++ )
                        cur_sol[m][n]=cur_provv[m][n];
                max=score;
            }
        }

    free(available);
    for (i=0;i<6;i++)
        free(permutation[i]);
    free(permutation);
    for (i=0;i<p.n;i++)
        free(cur_provv[i]);
    free(cur_provv);

    return(max);
}


int checkEdge(puzzle p, int **cur_sol,int i,int j){

    int roti=cur_sol[i][1];
    int rotj=cur_sol[j][1];

    int **col=p.colours;

    int N=i-p.w_size;
    int S=i+p.w_size;
    int E=i+1;
    int W=i-1;

    int coli_S=(0+3*roti)%4;
    int coli_W=(1+3*roti)%4;
    int coli_N=(2+3*roti)%4;
    int coli_E=(3+3*roti)%4;

    int colj_S=(0+3*rotj)%4;
    int colj_W=(1+3*rotj)%4;
    int colj_N=(2+3*rotj)%4;
    int colj_E=(3+3*rotj)%4;

    if (N==j) {
        if (col[cur_sol[i][0]][coli_N]==col[cur_sol[j][0]][colj_S])
            return 1;
        else
            return 0;
    }

    if (S==j) {
        if (col[cur_sol[i][0]][coli_S]==col[cur_sol[j][0]][colj_N])
            return 1;
        else
            return 0;
    }

    if (E==j) {
        if (col[cur_sol[i][0]][coli_E]==col[cur_sol[j][0]][colj_W])
            return 1;
        else
            return 0;
    }

    if (W==j) {
        if (col[cur_sol[i][0]][coli_W]==col[cur_sol[j][0]][colj_E])
            return 1;
        else
            return 0;
    }
    return(-1);
}

int haveBorderColour(puzzle p, int piece){

    int **col=p.colours,i,border;

    for (i=border=0;i<4;i++)
        if (col[piece][i]==0)
            border++;

    return(border);
}

int isBorder(puzzle p,int **cur_provv,int pos) {

    int score;

    int N=pos-p.w_size;
    int S=pos+p.w_size;
    int E=pos+1;
    int W=pos-1;

    score=0;

    if (N<0)          //North side is a border
       score++;

    if ((E%p.w_size)==0)      //Est side is a border
        score++;

    if ((((W%p.w_size)==(p.w_size-1)) || (W%p.w_size)==-1))      //West side is a border
        score++;

    if (S>=p.n)            //South side is a border
        score++;

    return(score);
}
