/************************************************************************
* FILENAME : printer.c
*
* DESCRIPTION :
*
*       Contains utility functions for printing moves and search paths
*       to the screen.
*
* PUBLIC FUNCTIONS :
*
*       void printMove(MOVE* move)
*       void printPath(STATE_NODE *input_node)
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/


/**
 * Function: printMove
 *
 * Prints the contents of the Move to the screen, including the Block number
 * and the direction that the block is moving.
 */

void printMove(MOVE* move) {

  printf("(%d,%s)\n",move->block_num, Move_Strings[move->direction]);

}


/**
 * Function: printPath
 *
 * Traverses the State Node Graph and recursively prints out each move from the
 * boards original state all the way down to the current <input_state>.
 */

void printPath(STATE_NODE *input_node) {

  if (input_node == NULL || input_node->parent == NULL) {
    return;
  }
  else
  {
    printPath(input_node->parent);
    printMove(input_node->move_from_parent);
  }

}
