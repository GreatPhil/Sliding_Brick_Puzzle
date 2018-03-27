/************************************************************************
* FILENAME : definitions.h
*
* DESCRIPTION :
*
*       Contains definitions needed for the sliding brick puzzle, including
*       representations of a Brick Move and State (containing the information
*       about the board, and the path cost to get to that state).  These State
*       Nodes are used to create the graphs for BFS and DFS traversal
*
* PUBLIC FUNCTIONS :
*
*       NONE
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Maximum number of chars / block (in input text file) */
#define MAX_BLOCK_NUM_CHARS 10

/* Implements the Block Move Diretion options */
typedef enum {UP, DOWN, LEFT, RIGHT} Move_Direction;
const char *Move_Strings[] = {"up","down","left","right"};

/* Represents a Block Move */
typedef struct MOVE {
    int            block_num;
    Move_Direction direction;
} MOVE;

/* State Node for the Breadth First Search (BFS) FIFO Queue and
   Depth First Search (DFS) FILO Stack.  The State Node is used
   to create the graph for searching across. */

typedef struct STATE_NODE {
    int    path_cost;          /* Cost from start of puzzle, i.e. g(n)     */
    int  **board_state;        /* 2-D array of the board contents          */
    MOVE  *move_from_parent;   /* Move leading to this board_state         */
    struct STATE_NODE *parent; /* parent state leading to this board_state */
    struct STATE_NODE *next;   /* Next pointer (used for FIFO and FILO)    */
} STATE_NODE;
