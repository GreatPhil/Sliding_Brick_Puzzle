/************************************************************************
* FILENAME : a_star_search.c
*
* DESCRIPTION :
*
*       Contains an implementation of an A* Search algorithm, to solve the
*       Sliding Brick Puzzle Game.  A*  Search includes a very basic heuristic
*       function and a method for selection of the optimal Node, based on
*       minimizing f(n) = g(n) + h(n), where g(n) is the path_cost to the
*       current_node and h(n) is the heuristic function to arrive at the goal.
*
*       NOTE: To ease implementation, the A* Search actually uses the same FIFO
*       as the BFS uses.  Therefore, to ensure no intermingling of search nodes in
*       the queue, the BFS must be run first, and the A* Search runs after draining
*       the BFS queue completely.
*
* PUBLIC FUNCTIONS :
*
*       int aStarSearch(int, int, int, int **)
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/

#define WORST_CASE_DISTANCE 1000

/**
 * Function: get_heuristic
 *
 * Returns a h(n) heuristic function that is an admissible and consistent
 * heuristic, based on the board dimensions and game's board_state.  Basically,
 * the heuristic returned here is the "Manhattan distance" from the 2-Block to
 * the Minus-1-Block (i.e. the Start Block to the End Block).
 */

int get_heuristic(int board_height, int board_width, int max_block_num, int **board_state) {

  int i,j = 0;

  /* Indexes of the Start and End Blocks */
  int start_block_row = -1;
  int start_block_col = -1;
  int end_block_row = -1;
  int end_block_col = -1;

  /* For Calculating Manhattan Distance */
  int row_range = 0;
  int col_range = 0;

  /* Find these Blocks in Question */
  for (i = 0; i < board_height; i++) {
    for (j = 0; j < board_width; j++) {

      /* Start Block Found */
      if (board_state[i][j] == 2) {
        if (i > start_block_row) {
          start_block_row = i;
        }
        if (j > start_block_col) {
          start_block_col = j;
        }
      }

      /* End Block Found */
      if (board_state[i][j] == -1) {
        if (i > end_block_row) {
          end_block_row = i;
        }
        if (j > end_block_col) {
          end_block_col = j;
        }
      }
    }
  }

  /* Calcuate Row Range */
  row_range = end_block_row - start_block_row;
  if (row_range < 0) {
    row_range = -row_range;
  }

  /* Calcuate Column Range */
  col_range = end_block_col - start_block_col;
  if (col_range < 0) {
    col_range = -col_range;
  }

  /* Return Manhattan Distance */
  return row_range + col_range;
}


/**
 * Function: getBestNode
 *
 * Returns a pointer to the Node that provides the lowest expected path from
 * start state to goal state (i.e. minimizes f(n) = g(h) + h(n)).
 */

STATE_NODE* getBestNode(int board_height, int board_width, int max_block_num, int **board_state) {

  STATE_NODE* search_node = bfs_fifo_head;
  STATE_NODE* shortest_node = NULL;

  int shortest_path = WORST_CASE_DISTANCE;
  int f_of_n = 0;
  int g_of_n = 0;
  int h_of_n = 0;

  /* Loop through the FIFO, finding the Node with the lowest f(n) */
  while (search_node != NULL) {

    /* Calculate f(n) */
    g_of_n = search_node->path_cost;
    h_of_n = get_heuristic(board_height, board_width, max_block_num, search_node->board_state);
    f_of_n = g_of_n + h_of_n;

    /* Update Shortest Path */
    if (f_of_n < shortest_path) {
      shortest_path = f_of_n;
      shortest_node = search_node;
    }

    search_node = search_node->next;
  }

  return shortest_node;

}


/**
 * Function: removeNode
 *
 * Removes the Node from the bfsQueue (really just makes the Queue skip over the Node)
 */

void removeNode(STATE_NODE* nodeToRemove) {

  STATE_NODE* current_node = bfs_fifo_head;
  STATE_NODE* next_node = NULL;

  /* If the first Node is the one to remove, just dequeue */
  if (compareStates(bfs_fifo_head->board_state, nodeToRemove->board_state)) {
    bfsDequeue();
    return;
  }

  /* Loop throught the FIFO looking for the Node to remove */
  while (current_node != NULL) {
    next_node = current_node->next;
    if (next_node != NULL && compareStates(next_node->board_state, nodeToRemove->board_state)) {
      current_node->next = next_node->next;
      return;
    }
    current_node = current_node->next;
  }
}


/**
 * Function: aStarSearch
 *
 * Performs an A* Search, i.e. travelling along the path that minimizes
 * f(n) = g(n) + h(n)s
 */

int aStarSearch(int board_height, int board_width, int max_block_num, int **board_state) {

  int i = 0;
  int num_moves = 0;
  MOVE *available_moves = NULL;
  STATE_NODE* current_state_node = NULL;

  int **next_board_state = NULL;
  int   next_board_depth = 0;

  char *next_board_hash  = NULL;
  int   hash_table_value = 0;

  /* Make sure the BFS queue is empty before we get started */
  drainQueue();

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

    /* Select the next State Node ==>  KEY TO THE A* SEARCH !!! */
    current_state_node = getBestNode(board_height, board_width, max_block_num, board_state);
    removeNode(current_state_node);

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
