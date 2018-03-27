/************************************************************************
* FILENAME : bfs_fifo_queue.c
*
* DESCRIPTION :
*
*       Implements a First-in-First-Out (FIFO) queue for use in the BFS algorith.
*       Note that the nodes in the queue contain information about their board_state
*       parent board_state, and move from the parent to the current board state.
*
* PUBLIC FUNCTIONS :
*
*       void bfsEnqueue(int **board_state, MOVE *input_move, STATE_NODE *parent)
*       STATE_NODE* bfsDequeue()
*       bool bfsQueueIsEmpty()
*       void drainQueue()
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/


/* Head and Tail of the BFS FIFO Queue */
STATE_NODE *bfs_fifo_head = NULL;
STATE_NODE *bfs_fifo_tail = NULL;


/**
 * Function: bfsEnqueue
 *
 * Creates a State Node with the given information (board_state, move, and
 * parent) and inserts the Node into the FIFO Queue, at the TAIL end.
 */

void bfsEnqueue(int **board_state, MOVE *input_move, STATE_NODE *parent) {

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

  /* Update Head if First Insertion */
  if (bfs_fifo_head == NULL){
    bfs_fifo_head = new_node;
  }

  /* Update Tail */
  if (bfs_fifo_tail == NULL){
    bfs_fifo_tail = new_node;
  }
  else
  {
    bfs_fifo_tail->next = new_node;
    bfs_fifo_tail = new_node;
  }

}


/**
 * Function: bfsDequeue
 *
 * Dequeues and returns a pointer to the State Node at the HEAD of the FIFO Queue.
 */

STATE_NODE* bfsDequeue() {

  STATE_NODE *return_node = bfs_fifo_head;
  STATE_NODE *new_head = bfs_fifo_head->next;
  bfs_fifo_head = new_head;
  return return_node;

}


/**
 * Function: bfsQueueIsEmpty
 *
 * Returns true is the FIFO Queue is empty
 */

bool bfsQueueIsEmpty() {
  return (bfs_fifo_head == NULL);
}


/**
 * Function: drainQueue
 *
 * Provides a way to empty out the bfsQueue
 */

void drainQueue() {
  while(!bfsQueueIsEmpty()) {
    bfsDequeue();
  }
}
