/************************************************************************
* FILENAME : depth_first_search.c
*
* DESCRIPTION :
*
*       Contains implementations for a generalized depth first search (DFS)
*       to solve the Sliding Brick Puzzle Game.  The general DFS is then used
*       to create more specifically: (1) a strict DFS function, a Depth-limited
*       DFS function, and an Iterative Deepening DFS function.
*
* PUBLIC FUNCTIONS :
*
*       int generalDepthFirstSearch (int, int, int, int**, bool, int)
*       int depthFirstSearch (int, int, int, int**)
*       int depthLimitedSearch (int, int, int, int**, int)
*       int interativeDeepeningSearch (int, int, int, int**)
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/


/**
 * Function: generalDepthFirstSearch
 *
 * Performs a depth first search on the input <board_state> with the given
 * <board_height>, <board_width>, and <max_block_num> (highest brick value).
 * If <depth_limited> is set to true, then the search will only search down to
 * a maximum number of steps, <max_depth>.  If a solution is found, this function
 * will return the path cost from the start state to the goal state along the
 * solution path.  If no solution path is found, this funcion returns -1.
 */

int generalDepthFirstSearch(int board_height, int board_width, int max_block_num,
                            int **board_state, bool depth_limited, int max_depth) {

  int i = 0;
  int num_moves = 0;
  MOVE *available_moves = NULL;
  STATE_NODE* current_state_node = NULL;

  int **next_board_state = NULL;
  int   next_board_depth = 0;

  char *next_board_hash  = NULL;
  int   hash_table_value = 0;

  /* Create Root State Node for the DFS */
  STATE_NODE root_state_node;
  root_state_node.path_cost = 0;
  root_state_node.board_state = board_state;
  root_state_node.move_from_parent = NULL;
  root_state_node.parent = NULL;
  root_state_node.next = NULL;

  /* Push Root Node onto DFS FILO Stack */
  dfsPushStack(root_state_node.board_state, NULL, NULL);

  /* Initialize Hash Table of Visited States (a.k.a. "Closed Set") */
  initStateHashTable(board_height, board_width, max_block_num);

  /* Add Root State to the Closed Set */
  insertIntoStateHashTable(getStateHashKey(board_state), 0);

  /* Loop through states in FILO Stack */
  while(!dfsStackIsEmpty()) {

    /* Dequeue the next State Node */
    current_state_node = dfsPopStack();

    /* Apply Check for Depth-Limited Search */
    if (!depth_limited || current_state_node->path_cost < max_depth) {

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

        /* Add to FILO Stack (if not already part of the closed Set) */
        next_board_hash = getStateHashKey(next_board_state);
        hash_table_value = getHashTableValue(next_board_hash);

        if (hash_table_value < 0) {
          dfsPushStack(next_board_state, &available_moves[i], current_state_node);
          insertIntoStateHashTable(next_board_hash, next_board_depth);
        }
        else if (hash_table_value > next_board_depth)
        {
          dfsPushStack(next_board_state, &available_moves[i], current_state_node);
          updateHashTableValue(next_board_hash, next_board_depth);
          free(next_board_hash);
        }
        else
        {
          free(next_board_hash);
        }

      }
    }

  }

  /* Return -1 indicating no soltion is found: */
  return -1;

}


/**
 * Function: depthFirstSearch
 *
 * Performs an UNLIMITED depth first search on the input <board_state> with the given
 * <board_height>, <board_width>, and <max_block_num> (highest brick value). If a
 * solution is found, this function will return the path cost from the start state to
 * the goal state along the solution path.  If no solution path is found, this
 * function returns -1.
 */

int depthFirstSearch(int board_height, int board_width, int max_block_num, int **board_state) {
  return generalDepthFirstSearch(board_height, board_width, max_block_num, board_state, false, 0);
}


/**
 * Function: depthLimitedSearch
 *
 * Performs a DEPTH-LIMITED depth first search on the input <board_state> with the given
 * <board_height>, <board_width>, and <max_block_num> (highest brick value). If a
 * solution is found, this function will return the path cost from the start state to
 * the goal state along the solution path.  If no solution path is found, this
 * function returns -1.
 */

int depthLimitedSearch(int board_height, int board_width, int max_block_num, int **board_state, int max_depth) {
  return generalDepthFirstSearch(board_height, board_width, max_block_num, board_state, true, max_depth);
}

/**
 * Function: interativeDeepeningSearch
 *
 * Performs a series of DEPTH-LIMITED depth first searches on the input <board_state>
 * with the given <board_height>, <board_width>, and <max_block_num> (highest brick
 * value) --  utilizing the iterative deepening seach algorith.  If a solution is found,
 * this function will return the path cost from the start state to the goal state along
 * the solution path.  If no solution path is found, this function returns -1.
 */

int interativeDeepeningSearch(int board_height, int board_width, int max_block_num, int **board_state) {

  int  search_depth  = 0;
  int  search_result = 0;
  bool goal_reached  = false;

  while (!goal_reached) {

    resetHashTable();
    search_result = depthLimitedSearch(board_height, board_width, max_block_num, board_state, ++search_depth);
    goal_reached = (search_result > 0);

  }

  return search_depth;
}
