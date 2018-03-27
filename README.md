# Sliding_Brick_Puzzle
This project implements Breadth First Search (BFS), Depth First Search (DFS), Iterative Deepening Search (IDS), and A-Star (A*) algorithms to solve the classic "Sliding Block" Puzzle Game.  This was written as a HW assignment for CS510 (Artificial Intelligence) @ Drexel University, Fall 2018.

### Summary

This project implements various classical search algorithms to solve yet another classical AI problem, the "Sliding Block" Puzzle (examples are here: _http://www.novelgames.com/en/sliding/_).

#### Rules of the game (from HW Assignment)

- A sliding brick puzzle is played on a rectangular w by h board (w cells wide and h cells tall). Each cell in the board can be either free, have a wall, or be the goal.
- On top of the board (over some of the free cells) there is a set of solid pieces (or bricks) that can be moved around the board. One of the bricks is special (the master brick).
- A move consists of sliding one of the bricks one cell up, down, left or right. Notice that
bricks collide with either walls or other bricks, so we cannot move a brick on top of
another. Bricks can only slide, they cannot rotate nor be flipped.
- To solve the puzzle, we have to find a sequence of moves that allows you to move the master brick on top of the goal cell(s). No other pieces are allowed to be placed on
top of the goal cell(s).

#### My Approach

The main code is located in sliding_brick_puzzle.c.  Within, the main() function, we can select which search algorithms to run (i.e. BFS, DFS, Iterative Deepening, or A*), and we can select an input starting puzzle state.  States are represented as CSV files in the text_files directory.  The meaning of the numbers in these CSV files are as such:

- -1: represents the goal location
- 0: means that the cell is empty
- 1: means that there is a wall
- 2: means that the master brick is on top of this cell
- Any number higher or equal than 3: represents each of the other bricks

Running the program will use the selected search algorithm, find the path to solution using the algorithm, print the list of moves to the screen, and report total execution time.  This is useful for comparing the results between the various algorithms.

### Compilation and Execution:

- Compilation is straight-forward, but a make file is provided as well.
- To compile, at the top directory level (the one with the Makefile), just type __make__.
- To execute after compiling, type __./sbp__.

### File Descriptions:

- "sliding_brick_puzzle.c"__ contains the main function and the implementation of the puzzle game state and generation and selection of movements (i.e. "actuators").

- "definitions.h"__ contains the structs that are used to represent brick movements and nodes in the various graph search functions.

- "breadth_first_search.c"__ implements the BFS.

- "depth_first_search.c"__ implements the DFS and IDS.

- "a_star_search.c"__ implements the A* Search Algorithm.

- The __"utilities"__ folder contains various utility functions needed to support (1) printing of the brick moves and a solution path from start state to goal state, (2) timing functions, (3) a FIFO queue (used for BFS), (4) a FILO stack (used for DFS), (5) a Hash Table used for tracking all of the visited nodes (i.e. the "closed set").

- The __"text_files"__ folder contains test sliding block puzzle files to be read into the sliding block puzzle game program.  These were provided as part of the assignment, and they provide a good way to compare and test the different search algorithms.
