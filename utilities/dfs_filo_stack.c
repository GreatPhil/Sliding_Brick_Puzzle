/************************************************************************
* FILENAME : dfs_filo_stack.c
*
* DESCRIPTION :
*
*       Implements a First-in-Last-Out (FILO) stack for use in the DFS algorith
*       Note that the nodes in the queue contain information about their board_state
*       parent board_state, and move from the parent to the current board state.
*
* PUBLIC FUNCTIONS :
*
*       void dfsPushStack(int **board_state, MOVE *input_move, STATE_NODE *parent)
*       STATE_NODE* dfsPopStack()
*       bool dfsStackIsEmpty()
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/


/* Head of the DFS FILO Stack (i.e. no tail is necessary since this is FILO) */
STATE_NODE *dfs_filo_head = NULL;


/**
 * Function: dfsPushStack
 *
 * Creates a State Node with the given information (board_state, move, and
 * parent) and pushes the Node onto the FILO Stack, at the HEAD end.
 */

void dfsPushStack(int **board_state, MOVE *input_move, STATE_NODE *parent) {

  /* Create the new State Node */
  STATE_NODE *new_node = malloc(sizeof(STATE_NODE));
  new_node->board_state = board_state;
  new_node->move_from_parent = input_move;
  new_node->parent = parent;
  new_node->next = NULL;

  /* Update Path Cost (for Depth Limited Search) */
  if (parent != NULL) {
    new_node->path_cost = parent->path_cost + 1;
  }
  else
  {
    new_node->path_cost = 0;
  }

  /* Push new State Node to Stack (i.e. Update Head) */
  if (dfs_filo_head == NULL){
    dfs_filo_head = new_node;
  }
  else
  {
    new_node->next = dfs_filo_head;
    dfs_filo_head = new_node;
  }

}


/**
 * Function: dfsPopStack
 *
 * Pops and returns a pointer to the State Node at the HEAD of the FILO Stack.
 */

STATE_NODE* dfsPopStack() {

  STATE_NODE *return_node = dfs_filo_head;
  STATE_NODE *new_head = dfs_filo_head->next;
  dfs_filo_head = new_head;
  return return_node;

}


/**
 * Function: dfsStackIsEmpty
 *
 * Returns true is the FILO Stack is empty
 */

bool dfsStackIsEmpty() {
  return (dfs_filo_head == NULL);
}
