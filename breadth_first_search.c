/************************************************************************
* FILENAME : breadth_first_search.c
*
* DESCRIPTION :
*
*       Contains an implementation of a breadth first search (BFS), to
*       solve the Sliding Brick Puzzle Game.
*
* PUBLIC FUNCTIONS :
*
*       int breadthFirstSearch(int, int, int, int **)
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/


/**
 * Function: breadthFirstSearch
 *
 * Performs a breadth first search on the input <board_state> with the given
 * <board_height>, <board_width>, and <max_block_num> (highest brick value).
 * If a solution is found, this function will return the path cost from the
 * start state to the goal state along the solution path.  If no solution path
 * is found, this funcion returns -1.
 */

int breadthFirstSearch(int board_height, int board_width, int max_block_num, int **board_state) {

  int i = 0;
  int num_moves = 0;
  MOVE *available_moves = NULL;
  STATE_NODE* current_state_node = NULL;

  int **next_board_state = NULL;
  int   next_board_depth = 0;

  char *next_board_hash  = NULL;
  int   hash_table_value = 0;

  /* Create Root State Node for the BFS */
  STATE_NODE root_state_node;
  root_state_node.path_cost = 0;
  root_state_node.board_state = board_state;
  root_state_node.move_from_parent = NULL;
  root_state_node.parent = NULL;
  root_state_node.next = NULL;

  /* Enqueue Root Node into BFS FIFO Queue */
  bfsEnqueue(root_state_node.board_state, NULL, NULL);

  /* Initialize Hash Table of Visited States (a.k.a. "Closed Set") */
  initStateHashTable(board_height, board_width, max_block_num);

  /* Add Root State to the Closed Set */
  insertIntoStateHashTable(getStateHashKey(board_state), 0);

  /* Loop through states in FIFO Queue */
  while(!bfsQueueIsEmpty()) {

    /* Dequeue the next State Node */
    current_state_node = bfsDequeue();

    /* Generate list of legal moves from Current State */
    num_moves = getAllAvailableMoves(current_state_node->board_state, &available_moves);

    for (i = 0; i < num_moves; i++) {

      /* Generate and Normalize Next State */
      next_board_state = applyMoveCloning(current_state_node->board_state, available_moves[i]);
      next_board_depth = current_state_node->path_cost + 1;
      normalizeState(next_board_state);

      /* Check if goal state is reached */
      if (checkGameComplete(next_board_state)) {

        /* Print path to goal state (and print the goal state) */
        printPath(current_state_node);
        printMove(&available_moves[i]);
        printState(next_board_state);
        free(next_board_state);
        return next_board_depth;
      }

      /* Add to FIFO Queue (if not already part of the closed Set) */
      next_board_hash = getStateHashKey(next_board_state);
      hash_table_value = getHashTableValue(next_board_hash);
      if (hash_table_value >= 0) {
        free(next_board_state);
        free(next_board_hash);
      }
      else
      {
        bfsEnqueue(next_board_state, &available_moves[i], current_state_node);
        insertIntoStateHashTable(next_board_hash, next_board_depth);
      }

    }
  }

  /* Return -1 indicating no soltion is found: */
  return -1;

}
