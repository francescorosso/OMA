/*greedy_rand.h*/
#ifndef _GREEDY_INCLUDE
#define _GREEDY_INCLUDE

#define MAX_LOCAL_SCORE 4       //Max score that can be calculate

typedef struct Puzzle {
	int w_size;
	int h_size;
	int n;
	int **colours;
} puzzle;

extern int basket_size;


/* greedyRandomized
*  Implementation of Greedy's algorithm.
*
*  p: puzzle
*  cur_sol: solution to be computed
*
*  return total score of the solution
*/
int greedyRandomized(puzzle p, int **cur_sol);


/* greedyRandomizedSpiral
*  Implementation of Greedy's algorithm. In order to complete the matrix's solution, this function follow a spiral way instead of a linear one
*  (for rows). In this way, the probability that a border piece isn't correctly positionated is very low.
*
*  p: puzzle
*  cur_sol: solution to be computed
*
*  return total score of the solution
*/

int greedyRandomizedSpiral(puzzle p, int **cur_sol);


/* choosePiece
*  Calculate local score in position pos of each pice with each rotation. Choose a random piece from the
*  basket (previous inizialized from proper function) and insert it (with the rotation that maximaze the score) in the current solution.
*
*  p: puzzle
*  cur_sol: solution to be computed
*  basket: vector fill up with the best score pieces. The number of pieces choose is predefined.
*  available: vector of sizePuzzle that marks if a pieces is already take
*  localscores: vector with the local scores of each pice
*  pos: location in wich evalutate each piece
*/
void choosePiece(puzzle p, int **cur_sol, int *basket, int *available, int *localscores, int pos);


/* takeKBestScore
*  Take #BasketSize best available piece and fill the basket
*
*  localscores: vector with the local scores of each pice
*  basket: vector fill up with the best score pieces. The number of pieces choose is predefined.
*  available: vector of sizePuzzle that marks if a pieces is already take
*  sizePuzzle: size of the puzzle
*
*  return number of piece in the basket
*/
int takeKBestScore(int *localscores,int *basket,int *available,int puzzleSize);


/* computeLocalScore
*  Compute local score of one piece
*
*  p: puzzle
*  cur_sol: solution to be computed
*  piece: piece numer
*  pos: position of piece in cur_sol
*  rot: rotation of the piece
*
*  return local score
*/
int computeLocalScore(puzzle p, int **cur_sol, int piece, int pos, int rot);


/* computeScore
*  Computes total score, and allocates a vector wrong[] of size wrong_size, with position of all wrong pieces
*
*  p: puzzle
*  cur_sol: solution to be computed
*  w_ptr:ptr to vector wrong[], where each element is position of a wrong piece
*  wsize_ptr: ptr to the number of wrong pieces found
*
*  return total score
*/
int computeScore(puzzle p, int **cur_sol, int *wrongs, int *wsize_ptr);


/* checkEdge
*  Check if piece in positions i and j doing score
*
*  p: puzzle
*  cur_sol: solution to be computed
*  i: position of first piece
*  j: position in second piece
*
*  return 1 if match 0 if not. -1 is returned when piece i and j aren't neighbors.
*/
int checkEdge(puzzle p, int **cur_sol,int i,int j);


//TODO Documentation

/* multiStart
*  Start Greedy algorithm with a pice pre-positionated according to certain algorithms to improve the result
*
*  p: puzzle
*  cur_sol: solution to be computed
*  multi_start: which algorithm perform:
*                    0 - start greedyRandomizedSpiral with all permutation of angle possible
*
*  return total score.
*/

int multiStartAnglePermutation(puzzle p, int **cur_sol);

int multiStartAnglePermutationPlusOne (puzzle p, int **cur_sol);

int multiStartAnglePermutationPlusP (puzzle p, int **cur_sol, int n_pice);

int multiStartRandom (puzzle p, int **cur_sol, int pices_number, int iteration);

#endif
