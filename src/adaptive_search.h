/*adaptive_search.h*/

#ifndef ADAPTIVE
#define ADAPTIVE

#include "greedy_rand.h"

int adaptiveSearch(puzzle p, int** cur_sol);
/*DEPRECATED. use computeScore instead*/
/*computeWrongs
* p: puzzle
* ptr:ptr to new array of int, where each element is position of a wrong piece
* scoreptr: ptr to the total score, computed by the function
* returns: total number of wrong elements */
int computeWrongs(puzzle p,int** cur_sol, int** ptr, int *scoreptr);

/*trySwapPieces
* compute the score the puzzle WOULD have, if swap occured
* p:puzzle
* i, j: pieces to be swap
* returns: new total score with the swap
*
*/
int trySwapPieces(puzzle p,int **cur_sol,int score, int i, int j, int *wrongs, int *wsize_ptr);

/*swapPieces
* swap the 2 pieces in position i, j in puzzle p and updates wrong[] array
*/
int swapPieces(puzzle p,int **cur_sol, int i, int j, int *wrong, int *wsize_ptr);

/* isNeighbor
*test if positions i and j in puzzle p are neighbors
* returns 0 if FALSE
*/

int isNeighbor(puzzle p, int i, int j);

/* updateWrongs
* check, for each neigbor of position i, its belonging to Wrongs set, and update it
*/
void updateWrongs(puzzle p, int **cur_sol, int *wrongs, int *wsize_ptr, int i);

/* addWrong
* add position i to Wrongs set, if not yet belonging
*/
void addWrong(puzzle p, int **cur_sol, int *wrongs, int *wsize_ptr, int i);

/*removeWrong
* remove position i from Wrongs set, if belonging
*/
void removeWrong(puzzle p, int **cur_sol, int *wrongs, int *wsize_ptr, int i);





#endif
