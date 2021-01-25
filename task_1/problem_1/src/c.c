#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000 /* maximum matrix size */
#define MAXWORKERS 8  /* maximum number of workers */
#define MAXVALUE 99
// #define DEBUG

typedef struct element
{
    int value;
    int row;
    int column;
} element;

pthread_mutex_t barrier;
pthread_mutex_t rowBarrier;

int numWorkers;              /* number of workers */
int numArrived = 0;          /* number who have arrived */
double start_time, end_time; /* start and end times */
int size, sum, maxValue, minValue, maxRow, maxColumn, minRow, minColumn, nextRow;
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

double read_timer()
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if (!initialized)
    {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

void update(int total, struct element *maxValues, struct element *minValues)
{
    sum += total;

    if (maxValue < maxValues->value)
    {
        maxValue = maxValues->value;
        maxRow = maxValues->row;
        maxColumn = maxValues->column;
    }

    if (minValue > minValues->value)
    {
        minValue = minValues->value;
        minRow = minValues->row;
        minColumn = minValues->column;
    }
}

void *Worker(void *arg)
{
    long myid = (long)arg;
    int total, i, j, row;

    element maxValues;
    element minValues;

#ifdef DEBUG
    printf("worker %ld (pthread id %ld) has started\n", myid, pthread_self());
#endif

    maxValues.value = -1;
    minValues.value = MAXVALUE + 1;
    total = 0;

    while (true)
    {

        //  if (nextRow >= MAXSIZE)
        //     break;

        pthread_mutex_lock(&rowBarrier);

        if (nextRow >= MAXSIZE)
        {
            pthread_mutex_unlock(&rowBarrier);
            break;
        }

        row = nextRow;
        nextRow++;

        pthread_mutex_unlock(&rowBarrier);

        for (j = 0; j < size; j++)
        {

            if (matrix[row][j] > maxValues.value)
            {
                maxValues.value = matrix[row][j];
                maxValues.row = row;
                maxValues.column = j;
            }

            if (matrix[row][j] < minValues.value)
            {
                minValues.value = matrix[row][j];
                minValues.row = row;
                minValues.column = j;
            }

            total += matrix[row][j];
        }
    }

    pthread_mutex_lock(&barrier);
    update(total, &maxValues, &minValues);
    pthread_mutex_unlock(&barrier);
}

int main(int argc, char *argv[])
{
    int i, j;
    long l;

    pthread_attr_t attr;
    pthread_t workerid[MAXWORKERS];

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    pthread_mutex_init(&barrier, NULL);
    pthread_mutex_init(&rowBarrier, NULL);

    /* read command line args if any */
    size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;

    if (size > MAXSIZE)
        size = MAXSIZE;

    if (numWorkers > MAXWORKERS)
        numWorkers = MAXWORKERS;

    /* initialize the matrix */
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            matrix[i][j] = rand() % MAXVALUE;

            /* print the matrix */
#ifdef DEBUG
    for (i = 0; i < size; i++)
    {
        printf("[ ");
        for (j = 0; j < size; j++)
        {
            printf(" %d", matrix[i][j]);
        }
        printf(" ]\n");
    }
#endif

    maxValue = -1;
    minValue = MAXVALUE + 1;
    sum = 0;
    nextRow = 0;

    start_time = read_timer();

    for (l = 0; l < numWorkers; l++)
        pthread_create(&workerid[l], &attr, Worker, (void *)l);

    for (l = 0; l < numWorkers; l++)
        pthread_join(workerid[l], NULL);

    end_time = read_timer();

    printf("The total is %d\n", sum);
    printf("The maximum value is %d at row %d and column %d\n", maxValue, maxRow + 1, maxColumn + 1);
    printf("The minimum value is %d at row %d and column %d\n", minValue, minRow + 1, minColumn + 1);
    printf("The execution time is %g sec\n", end_time - start_time);
}
