/************************************************************************
* FILENAME : run_timer.c
*
* DESCRIPTION :
*
*       Implements a timer for use in measuring execution time for the various
*       search algorithms, and a method for printing the elapsed time.
*
* PUBLIC FUNCTIONS :
*
*      void startRunTimer()
*      void endRunTimer()
*      double getElapsedRunTime()
*      void printElapsedRunTime()
*
* AUTHOR : Philip Cheng
* DATE   : 18 October 2017
*
************************************************************************/

#include <time.h>

static clock_t start_time = 0;
static clock_t end_time   = 0;


/**
 * Function: startRunTimer
 *
 * Starts the Run Timer, given the current system clock time.
 */

void startRunTimer() {
  start_time = clock();
}


/**
 * Function: endRunTimer
 *
 * Ends the Run Timer, given the current system clock time.
 */

void endRunTimer() {
  end_time = clock();
}


/**
 * Function: getElapsedRunTime
 *
 * Calculates and returns the elapsed time (in seconds).
 */

double getElapsedRunTime() {
  return (double) (end_time - start_time) / CLOCKS_PER_SEC;
}


/**
 * Function: printElapsedRunTime
 *
 * Prints the elapsed time to the screen in the format of seconds and fractions
 * of a second (e.g. 2.534 --> "2 seconds and 534/1000").
 */

void printElapsedRunTime() {
  int seconds = (int) getElapsedRunTime();
  int milliseconds = (int) ((getElapsedRunTime() - (double) seconds) * 1000);
  printf("(%d seconds and %d/1000)", seconds, milliseconds);
}
