/************************************************************************
* FILENAME : state_hash_table.c
*
* DESCRIPTION :
*
*       Implements a hashtable for holding states that have been searched
*       already.  This is used in the BFS and DFS algorithms to make sure that
*       loops are not re-searched.  In the DEPTH-first search, a Hash Table Node
*       may actually be searched again IFF a new path to the node's state is
*       found with a lower path cost.  Therefore, the Key-Value pairs for this
*       hash table are a string encoding of the game's board_state as the key,
*       and the path-cost to get to that board_state from the starting state as
*       the value.
*
* PUBLIC FUNCTIONS :
*
*       void initStateHashTable(int board_height, int board_width, int max_block_num)
*       void resetHashTable()
*       char *getStateHashKey(int **input_state)
*       int getHashKeyIndex(char *hashkey)
*       void insertIntoStateHashTable(char *key, int value)
*       int getHashTableValue(char *key)
*       void updateHashTableValue(char *key, int value)
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/

#define STATE_HASH_TABLE_SIZE 1000

/* Nodes in Hash Table */
typedef struct HASH_TABLE_NODE {
    char  *key;
    int    value;
    struct HASH_TABLE_NODE *next;
} HASH_TABLE_NODE;

/* State Parameters (used for Hash functions) */
static int state_hashtable_board_height  = 0;
static int state_hashtable_board_width   = 0;
static int state_hashtable_max_block_num = 0;

/* Number of Nodes in the Hash Table */
static int hash_table_node_count = 0;

/* Hash Table Implementation */
HASH_TABLE_NODE *state_hashtable[STATE_HASH_TABLE_SIZE] = {NULL};


/**
 * Function: initStateHashTable
 *
 * Sets up the hash table's parameters and sets each slot in the table to NULL.
 */

void initStateHashTable(int board_height, int board_width, int max_block_num) {

  int i = 0;

  state_hashtable_board_height  = board_height;
  state_hashtable_board_width   = board_width;
  state_hashtable_max_block_num = max_block_num;

  hash_table_node_count = 0;

  for (i = 0; i < STATE_HASH_TABLE_SIZE; i++) {
    state_hashtable[i] = NULL;
  }

}


/**
 * Function: initStateHashTable
 *
 * Clears the hash table, cleans up memory, and resets each slot in the table
 * back to NULL.  This is usefule for preparing the Hash Table for a brand new
 * search, or a new level of Iterative Deepening Search (IDS).
 */

void resetHashTable() {

  int i = 0;
  HASH_TABLE_NODE *current_node = NULL;
  HASH_TABLE_NODE *next_node = NULL;

  /* Reset Number of Nodes in Hash Table back to Zero */
  hash_table_node_count = 0;

  /* Clean Up Memory */
  for (i = 0; i < STATE_HASH_TABLE_SIZE; i++) {
    current_node = state_hashtable[i];
    while (current_node != NULL) {
      next_node = current_node->next;
      free(current_node->key);
      free(current_node);
      current_node = next_node;
    }
  }
}


/**
 * Function: getStateHashKey
 *
 * Creates a simple UNIQUE Hash Key (string) given an <input_state> board_state.
 * Accomplishes this by encoding each block value on a predetermined board size
 * to a letter (i.e. 0 --> 'A', 1 --> 'B', 2 --> 'C', etc. )
 */

char *getStateHashKey(int **input_state) {

  int  i,j   = 0;
  int  index = 0;

  char *hashkey = malloc(sizeof(char) * ((state_hashtable_board_height - 2) * (state_hashtable_board_width - 2) + 1));

  for (i = 1; i < state_hashtable_board_height - 1; i++) {
    for (j = 1; j < state_hashtable_board_width - 1; j++) {
      hashkey[index++] = (char) input_state[i][j] + 'A';
    }
  }

  hashkey[index] = '\0';
  return hashkey;

}


/**
 * Function: getHashKeyIndex
 *
 * Calculates the unique Hash Key's index within the Hashtable, as a weighted
 * sum of the Hash Key's char values -- all modulo the size of the Hash Table.
 */

int getHashKeyIndex(char *hashkey) {

  int i = 0;
  int running_total = 0;
  int key_len = strlen(hashkey);

  for (i = 0; i < key_len; i++) {
    running_total += hashkey[i] * (i+1);
  }

  return running_total % STATE_HASH_TABLE_SIZE;

}


/**
 * Function: insertIntoStateHashTable
 *
 * Inserts a Key-Value pair into the Hash Table
 */

void insertIntoStateHashTable(char *key, int value) {

  /* Get Hash Table Index */
  int index = getHashKeyIndex(key);
  HASH_TABLE_NODE *location = state_hashtable[index];

  /* Create New State Node */
  HASH_TABLE_NODE *new_state = malloc(sizeof(HASH_TABLE_NODE));

  /* Fill in New State Data */
  new_state->key   = key;
  new_state->value = value;
  new_state->next  = NULL;

  /* Insert New State Node into Hash Table */
  if (location == NULL) {
    state_hashtable[index] = new_state;
  }
  else
  {
    while (location->next != NULL) {
      location = location->next;
    }
    location->next = new_state;
  }

  /* Update counter */
  hash_table_node_count++;

}


/**
 * Function: getHashTableValue
 *
 * Returns a Value, given a Key.  Since the values that are stored are path-costs,
 * all of the values must be non-negative.  Therefore, if the key is not found, this
 * function will return -1, to indicate that the Key-Value pair is not in the table.
 */

int getHashTableValue(char *key) {

  /* Get Hash Table Index */
  HASH_TABLE_NODE *search_node = state_hashtable[getHashKeyIndex(key)];

  /* Search Hash Table for Key */
  while (search_node != NULL) {
    if (strcmp(search_node->key, key) == 0) {
      return search_node->value;
    }
    search_node = search_node->next;
  }

  /* Otherwise, return -1 */
  return -1;

}


/**
 * Function: updateHashTableValue
 *
 * Updates the value for the given Key.  This assumes that the Key was already
 * in the hash table.  This is useful for depth-first-search, when a new path
 * to an already visited board_state is found, but the new path is shorter.
 */

void updateHashTableValue(char *key, int value) {

  /* Get Hash Table Index */
  HASH_TABLE_NODE *search_node = state_hashtable[getHashKeyIndex(key)];

  /* Search Hash Table for Key */
  while (search_node != NULL) {
    if (strcmp(search_node->key, key) == 0) {
      search_node->value = value;
      return;
    }
    search_node = search_node->next;
  }

  return;

}
