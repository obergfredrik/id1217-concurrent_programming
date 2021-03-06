# Quicksort

The quicksort algorithm sorts the list of numbers by first dividing the list into two sublists, so that all the numbers if one sublist are smaller than all the numbers in the other sublist. This is done by selecting one number (called a pivot) against which all other number are compared: the numbers which are less then the pivot are placed in one sublist, the numbers which more than the pivot are placed in another sublist. The pivot can be either placed in one sublist or could be withheld and placed in its final position. 

Develop a parallel multithreaded program (in C using Pthreads, or in Java) with recursive parallelism that implements the quicksort algorithm for sorting an array of n values. 

Performance evaluation: Measure and print also the execution time of your program using the times function or the gettimeofday function (see how it is done in matrixSum.c). To calculate the execution time, read the clock after you have initialized all variables and just before you create the threads. Read the clock again as soon as the computation is complete and the worker threads have terminated.