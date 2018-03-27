 /************************************************************************
 * FILENAME : sliding_brick_puzzle.c
 *
 * DESCRIPTION :
 *
 *       Contains the main implemtation of the sliding brick puzzle game. This includes
 *       functions for loading the game, printing game state, cloning game state, and
 *       checking to see if the game has ended.
 *
 *       This file also contains functions for manipulating the game state, including
 *       getting a list of available moves, applying those moves, checking if two states
 *       are equivalent, normalizing a state, and performing a random walk.
 *
 * PUBLIC FUNCTIONS :
 *
 *       void  loadGameState(char *filename)
 *       void  printState(int **board_state)
 *       void  printGameState()
 *       int **cloneGameState(int ** orig_state)
 *       bool  checkGameComplete(int ** game_state)
 *       int   getAvailableMoves(int **input_state, int piece_num, MOVE **available_moves)
 *       int   getAllAvailableMoves(int **input_state, MOVE **available_moves)
 *       void  applyMove(int **input_state, MOVE move)
 *       int **applyMoveCloning(int **input_state, MOVE move)
 *       bool  compareStates(int **state_a, int **state_b)
 *       void  normalizeState(int **input_state)
 *       void  randomWalk(int **board_state, int N)
 *
 * AUTHOR : Philip Cheng
 * DATE   : 18 October 2017
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Includes Move and Graph definitions */
#include "definitions.h"

/* Include Utilities Functions */
#include "utilities/printer.c"
#include "utilities/run_timer.c"
#include "utilities/bfs_fifo_queue.c"
#include "utilities/dfs_filo_stack.c"
#include "utilities/state_hash_table.c"

/* Function Declarations */
void  loadGameState(char *filename);
void  printState(int **board_state);
void  printGameState();
int **cloneGameState(int ** orig_state);
bool  checkGameComplete(int ** game_state);
int   getAvailableMoves(int **input_state, int piece_num, MOVE **available_moves);
int   getAllAvailableMoves(int **input_state, MOVE **available_moves);
void  applyMove(int **input_state, MOVE move);
int **applyMoveCloning(int **input_state, MOVE move);
bool  compareStates(int **state_a, int **state_b);
void  normalizeState(int **input_state);
void  randomWalk(int **board_state, int N);

/* Includes A-Star, BFS, and DFS Search Implementations */
#include "a_star_search.c"
#include "breadth_first_search.c"
#include "depth_first_search.c"

/*******************/
/* Game State Info */
/*******************/

int   board_width   = 0;     /* Number of columns on the board           */
int   board_height  = 0;     /* Number of rows on the board              */
int   max_block_num = 0;     /* Number of the highest block on the board */
int **board_state   = NULL;  /* 2-D Array of board's contents            */


/**
 * Function: loadGameState
 *
 * Loads a game state from disk given an ASCII text file: <filename>. If file is
 * not found, this function simply returns, without any additional error message.
 * It is assumed that the user provides a valid file name, to enable loading of
 * a sliding brick puzzle map.
 */

void loadGameState(char *filename) {

  char input_buffer[MAX_BLOCK_NUM_CHARS] = {0};
  char input_char = 0;
  char *dummy_ptr = 0;
  int  i,j,k      = 0;
  int  block_num  = 0;

  /* Open File */
	FILE *file = fopen(filename, "rb");
  if (file == NULL) { return; }

  /* Get Board Width */
  i = 0;
  do {
    input_char = getc(file);
    input_buffer[i++] = input_char;
  } while (input_char != ',');

  board_width = (int) strtol(input_buffer, &dummy_ptr, 10);
  memset(&input_buffer[0], 0, sizeof(input_buffer));

  /* Get Board Height */
  i = 0;
  do {
    input_char = getc(file);
    input_buffer[i++] = input_char;
  } while (input_char != ',');

  board_height = (int) strtol(input_buffer, &dummy_ptr, 10);
  memset(&input_buffer[0], 0, sizeof(input_buffer));

  /* Create Board */
  board_state  = malloc(sizeof(int *) * board_height);
  for (i = 0; i < board_height; i++) {
    board_state[i] = malloc(sizeof(int) * board_width);
  }

  /* Fill in Board */
  for (j = 0; j < board_height; j++) {
    for (k = 0; k < board_width; k++) {

      i = 0;
      do {

        input_char = getc(file);

        /* Check for early EOF */
        if (input_char == EOF) {
          printf("EOF happened prematurely.\n");
          return;
        }

        /* Get next Input Char */
        if (input_char != '\n') {
          input_buffer[i++] = input_char;
        }

      } while (input_char != ',');

      /* Update square on Board (and zero out the input_buffer) */
      block_num = (int) strtol(input_buffer, &dummy_ptr, 10);
      memset(&input_buffer[0], 0, sizeof(input_buffer));
      board_state[j][k] = block_num;

      /* Update Max Block Num */
      if (block_num > max_block_num) {
        max_block_num = block_num;
      }

    }
  }
}


/**
 * Function: clearGameState
 *
 * Cleans up memory allocated by the game <board_state> and resets board values
 * to their default values, so that the game state is ready to start a new game.
 */

void clearGameState() {

  int i = 0;

  /* Perform Garbage Collection */
  for (i = 0; i < board_height; i++) {
    free(board_state[i]);
  }
  free(board_state);

  /* Reset Game Paramenters */
  board_height  = 0;
  board_width   = 0;
  max_block_num = 0;
  board_state   = NULL;

}


/**
 * Function: printState
 *
 * Prints an input game state <board_state> to the screen.
 */

void printState(int **board_state) {

  int  i,j = 0;

  printf("%d,%d,\n", board_width, board_height);

  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      printf("%d,", board_state[i][j]);
    }
    printf("\n");
  }
  printf("\n");

}


/**
 * Function: printGameState
 *
 * Prints an the current game state to the screen.
 */

void printGameState() {
  printState(board_state);
}


/**
 * Function: cloneGameState
 *
 * Creates and returns a copy of an input game state <orig_state>.
 */

int **cloneGameState(int ** orig_state) {

  int i,j = 0;
  int **new_board = 0;

  /* Create a New Board */
  new_board  = malloc(sizeof(int *) * board_height);
  for (i = 0; i < board_height; i++) {
    new_board[i] = malloc(sizeof(int) * board_width);
  }

  /* Copy Board Contents */
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      new_board[i][j] = orig_state[i][j];
    }
  }

  return new_board;

}


/**
 * Function: checkGameComplete
 *
 * Returns true if the game state is a SOLVED state, i.e. no -1 blocks are
 * included in the game's state because they are presumably covered by a 2-block.
 */

bool checkGameComplete(int ** game_state) {

  int i,j = 0;

  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      if (game_state[i][j] == -1) {
        return false;
      }
    }
  }
  return true;

}


/**
 * Function: getAvailableMoves
 *
 * Searches the <input_state> for all possible legal moves generated by moving
 * block # <piece_num>.  Returns a pointer to an array of moves.
 */

int getAvailableMoves(int **input_state, int piece_num, MOVE **available_moves) {

  /* Temporary Variables */
  int   i,j = 0;
  MOVE* local_available_moves = NULL;
  int   num_available_moves   = 4;

  /* Move Checkers */
  bool piece_found = false;
  bool up_legal    = true;
  bool down_legal  = true;
  bool left_legal  = true;
  bool right_legal = true;

  /* Scan board looking for piece matching <piece_num> */
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      if (input_state[i][j] == piece_num) {

        piece_found = true;

        /* Check if Up Direction is a Legal Move */
        if (up_legal == true) {
          if (i == 0 ||
             (piece_num == 2 &&
               input_state[i - 1][j] != 0  &&
               input_state[i - 1][j] != -1 &&
               input_state[i - 1][j] != piece_num) ||
             (piece_num != 2 &&
               input_state[i - 1][j] != 0  &&
               input_state[i - 1][j] != piece_num)) {

            up_legal = false;
            num_available_moves--;

          }
        }

        /* Check Down Direction */
        if (down_legal == true) {
          if (i == (board_height - 1) ||
             (piece_num == 2 &&
               input_state[i + 1][j] != 0  &&
               input_state[i + 1][j] != -1 &&
               input_state[i + 1][j] != piece_num) ||
             (piece_num != 2 &&
               input_state[i + 1][j] != 0  &&
               input_state[i + 1][j] != piece_num)) {

            down_legal = false;
            num_available_moves--;

          }
        }

        /* Check Left Direction */
        if (left_legal == true) {
          if (j == 0 ||
             (piece_num == 2 &&
               input_state[i][j - 1] != 0  &&
               input_state[i][j - 1] != -1 &&
               input_state[i][j - 1] != piece_num) ||
             (piece_num != 2 &&
               input_state[i][j - 1] != 0  &&
               input_state[i][j - 1] != piece_num)) {

            left_legal = false;
            num_available_moves--;

          }
        }

        /* Check Right Direction */
        if (right_legal == true) {

          if (j == (board_width - 1) ||
             (piece_num == 2 &&
               input_state[i][j + 1] != 0  &&
               input_state[i][j + 1] != -1 &&
               input_state[i][j + 1] != piece_num) ||
             (piece_num != 2 &&
               input_state[i][j + 1] != 0  &&
               input_state[i][j + 1] != piece_num)) {

            right_legal = false;
            num_available_moves--;

          }
        }
      }
    }
  }

  if (piece_found) {

    if (num_available_moves > 0) {

      i = 0;
      local_available_moves = malloc(sizeof(MOVE) * num_available_moves);

      if (up_legal) {
        local_available_moves[i].block_num = piece_num;
        local_available_moves[i].direction = UP;
        i++;
      }

      if (down_legal) {
        local_available_moves[i].block_num = piece_num;
        local_available_moves[i].direction = DOWN;
        i++;
      }

      if (left_legal) {
        local_available_moves[i].block_num = piece_num;
        local_available_moves[i].direction = LEFT;
        i++;
      }

      if (right_legal) {
        local_available_moves[i].block_num = piece_num;
        local_available_moves[i].direction = RIGHT;
        i++;
      }

      *available_moves = local_available_moves;

    }
    else
    {
      available_moves = NULL;
    }
  }

  return num_available_moves;

}


/**
 * Function: getAllAvailableMoves
 *
 * Searches the <input_state> for all possible legal moves generated by moving
 * any moveable block in the game state.  Returns a pointer to an array of moves.
 */

/* Gets the list of ALL available moves for the board state */
int getAllAvailableMoves(int **input_state, MOVE **available_moves) {

  int    i,j,k = 0;
  int    num_total_available_moves = 0;
  MOVE  *local_available_moves     = 0;
  MOVE **available_moves_per_block = malloc(sizeof(MOVE*));
  int    num_moves = 0;

  /* Count Number of Total Available Moves */
  for (i = 2; i <= max_block_num; i++) {
    num_moves = getAvailableMoves(input_state, i, available_moves_per_block);
    num_total_available_moves += num_moves;
  }

  /* Allocate memory for New Array (containing all moves for all blocks) */
  local_available_moves = malloc(sizeof(MOVE) * num_total_available_moves);

  /* Copy moves to New Array */
  i = 0;
  for (j = 2; j <= max_block_num; j++) {
    num_moves = getAvailableMoves(input_state, j, available_moves_per_block);
    for (k = 0; k < num_moves; k++){
      local_available_moves[i++] = (*available_moves_per_block)[k];
    }
  }

  /* Update Output */
  *available_moves = local_available_moves;
  return num_total_available_moves;

}


/**
 * Function: applyMove
 *
 * Applies the <move> to the game defined by <input_state>, and updates the
 * original <input state> contents with the game's next state.
 */

void applyMove(int **input_state, MOVE move) {

  int i,j = 0;
  int temp_board[board_height][board_width];

  /* Copy state to the temp_board (minus the block that will move) */
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      if (input_state[i][j] == move.block_num) {
        temp_board[i][j] = 0;
      }
      else
      {
        temp_board[i][j] = input_state[i][j];
      }
    }
  }

  /* Copy a moved version of the moving block to the temp_board */
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      if (input_state[i][j] == move.block_num) {

        switch (move.direction) {

          case UP:
            temp_board[i-1][j] = move.block_num;
            break;

          case DOWN:
            temp_board[i+1][j] = move.block_num;
            break;

          case LEFT:
            temp_board[i][j-1] = move.block_num;
            break;

          case RIGHT:
            temp_board[i][j+1] = move.block_num;
            break;

          default:
            break;

        }
      }
    }
  }

  /* Overwrite the original state */
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      input_state[i][j] = temp_board[i][j];
    }
  }

}


/**
 * Function: applyMoveCloning
 *
 * Applies the <move> to a clone of the game defined by <input_state>, and returns
 * the cloned version of the next game state (with the move applied).
 */

int **applyMoveCloning(int **input_state, MOVE move) {

  int** board_clone = cloneGameState(input_state);
  applyMove(board_clone, move);
  return board_clone;

}


/**
 * Function: compareStates
 *
 * Performs strict block-by-block comparison of two states, and returns true
 * if the two states are equal, and false if they are not.
 */

bool compareStates(int **state_a, int **state_b) {

  int i,j = 0;
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      if (state_a[i][j] != state_b[i][j]) {
        return false;
      }
    }
  }
  return true;
}


/**
 * Function: normalizeState
 *
 * "Normalizes" an <input_state>, by renumbering block numbers > 2 with new block
 * numbers arranged in incrementing numbers from the top-left to the bottom-right.
 */

void normalizeState(int **input_state) {

  int i,j = 0;

  int  block_num = 0;
  bool *block_seen = malloc(sizeof(bool) * max_block_num);

  int *remap = malloc(sizeof(int) * max_block_num);
  int remap_counter = 3;

  /* Initialize block_seen */
  for (i = 0; i <= max_block_num; i++) {
    block_seen[i] = false;
    remap[i] = 0;
  }

  /* Create Re-map Plan */
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      block_num = input_state[i][j];
      if (block_num > 2 && !block_seen[block_num]) {
        block_seen[block_num] = true;
        remap[block_num] = remap_counter++;
      }
    }
  }

  /* Re-map, to create Normalized State */
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {
      if (input_state[i][j] > 2) {
        input_state[i][j] = remap[input_state[i][j]];
      }
    }
  }

}


/**
 * Function: randomWalk
 *
 * Performs a Random Walk with N steps in the game, given an input <board_state>
 * as a starting point.  The Random Walk ends when either the game is solved, or
 * the number of steps taken by the Random Walk reaches N.
 */

void randomWalk(int **board_state, int N) {

  int num_steps = 0;
  int num_moves = 0;
  int rand_num  = 0;

  MOVE **available_moves = malloc(sizeof(MOVE*));

  bool goal_reached = checkGameComplete(board_state);

  /* Set Random Seed */
  srand(time(NULL));

  /* Normalize and print the initial state */
  normalizeState(board_state);
  printState(board_state);

  while (num_steps < N && !goal_reached) {

    /* Generate all moves from initial_state */
    num_moves = getAllAvailableMoves(board_state, available_moves);

    /* Select and Execute one move randomly */
    rand_num = rand() % num_moves;
    applyMove(board_state, (*available_moves)[rand_num]);
    printMove(&((*available_moves)[rand_num]));
    printf("\n");

    /* Normalize and print the new state */
    normalizeState(board_state);
    printState(board_state);

    /* Check if Goal is reached */
    goal_reached = checkGameComplete(board_state);

    num_steps++;

  }

  return;

}


/***************************************************************************
 * MAIN FUNCTION
 ***************************************************************************/

int main() {

  int  path_cost = 0;
  char file_name[] = "text_files/SBP-level3.txt";

  /* Select which Search Algorithms to Run: */
  bool test_random = true;
  bool test_bfs    = true;
  bool test_dfs    = true;
  bool test_ids    = true;
  bool test_ass    = false;

  /* Random Walk Example */
  if (test_random) {
    loadGameState(file_name);
    randomWalk(board_state, 3);
    clearGameState();
  }

  /* Breadth First Search */
  if (test_bfs) {
    loadGameState(file_name);
    initStateHashTable(board_height, board_width, max_block_num);
    startRunTimer();
    path_cost = breadthFirstSearch(board_height, board_width, max_block_num, board_state);
    endRunTimer();
    printf("%d ", hash_table_node_count);
    printElapsedRunTime();
    printf(" %d\n\n", path_cost);
    resetHashTable();
    clearGameState();
  }

  /* Depth First Search */
  if (test_dfs) {
    loadGameState(file_name);
    initStateHashTable(board_height, board_width, max_block_num);
    startRunTimer();
    path_cost = depthFirstSearch(board_height, board_width, max_block_num, board_state);
    endRunTimer();
    printf ("%d ", hash_table_node_count);
    printElapsedRunTime();
    printf(" %d\n\n", path_cost);
    resetHashTable();
    clearGameState();
  }

  /* Iterative Deepening Depth First Search */
  if (test_ids) {
    loadGameState(file_name);
    initStateHashTable(board_height, board_width, max_block_num);
    startRunTimer();
    path_cost = interativeDeepeningSearch(board_height, board_width, max_block_num, board_state);
    endRunTimer();
    printf ("%d ", hash_table_node_count);
    printElapsedRunTime();
    printf(" %d\n\n", path_cost);
    resetHashTable();
    clearGameState();
  }

  /* A-Star Search */
  if (test_ass) {
    loadGameState(file_name);
    initStateHashTable(board_height, board_width, max_block_num);
    startRunTimer();
    path_cost = aStarSearch(board_height, board_width, max_block_num, board_state);
    endRunTimer();
    printf ("%d ", hash_table_node_count);
    printElapsedRunTime();
    printf(" %d\n\n", path_cost);
    resetHashTable();
    clearGameState();
  }

  return 0;

}
