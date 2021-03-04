#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define GRID 100
#define ITERATIONS 100
#define WORKERS 4

double **new;
double **grid;

double start_time, end_time, maxDiff, temp;
int gridSize, iterations, boundary, workers;
FILE * file;

void allocateGrids()
{
    grid = malloc(boundary * sizeof(double *));
    new = malloc(boundary * sizeof(double *));
    for (size_t i = 0; i < boundary; i++)
    {
        grid[i] = (double *)malloc(boundary * sizeof(double));
        new[i] = (double *)malloc(boundary * sizeof(double));
    }
}

void initializeGrids()
{
    for (int i = 0; i < boundary; i++)
        for (int j = 0; j < boundary; j++)
            if (i == 0 || j == 0 || i == boundary - 1 || j == boundary - 1)
            {
                grid[i][j] = 1;
                new[i][j] = 1;
            }
            else
            {
                grid[i][j] = 0;
                new[i][j] = 0;
            }
}

void findMaxDiff()
{
    int i, j;
    maxDiff = 0;

    #pragma omp parallel for private(j,temp)
    for (i = 1; i < gridSize; i++)
        for (j = 1; j < gridSize; j++)
        {
            temp = grid[i][j] - new[i][j];
            if (temp < 0)
                temp = -temp;
            if (temp > maxDiff){
                #pragma omp critical
                {
                if(temp > maxDiff)
                    maxDiff = temp;
                }
            }
        }
}

void solveGrid()
{
    int i, j, k;
    for (i = 0; i < iterations; i++){

        #pragma omp parallel
        {   
            #pragma omp for private(k)
            for (j = 1; j < gridSize; j++)
                for (k = 1; k < gridSize; k++)
                    new[j][k] = (grid[j - 1][k] + grid[j + 1][k] + grid[j][k - 1] + grid[j][k + 1]) * 0.25;
            
            #pragma omp for private(k)
            for (j = 1; j < gridSize; j++)
                for (k = 1; k < gridSize; k++)
                    grid[j][k] = (new[j - 1][k] + new[j + 1][k] + new[j][k - 1] + new[j][k + 1]) * 0.25;
        }
    }
}
void output()
{
    printf("Grid size: %d\tIterations: %d\tTime: %g\tMaxDiff: %g\n", gridSize, iterations, end_time - start_time, maxDiff);
   
    file = fopen("output/parJacobi.txt", "w");

    for (int i = 0; i < boundary; i++)
        for (size_t j = 0; j < boundary; j++)
        {
            fprintf(file, "%.4f ", grid[i][j]);
            if (j == boundary - 1)
                fprintf(file, "\n");
        }

    fclose(file);
}

void initiate()
{

    allocateGrids();

    for (int i = 0; i < 5; i++)
    {
        initializeGrids();

        start_time = omp_get_wtime();
        solveGrid();
        findMaxDiff();
        end_time = omp_get_wtime();

        output();
    }
}

int main(int argc, char *argv[])
{
    gridSize = (argc > 1) ? atoi(argv[1]) : GRID;
    iterations = (argc > 2) ? atoi(argv[2]) : ITERATIONS;
    workers = (argc > 3) ? atoi(argv[3]) : WORKERS;

    if (gridSize > GRID)
        gridSize = GRID;
    if (iterations > ITERATIONS)
        iterations = ITERATIONS;
    if (workers > WORKERS)
        workers = WORKERS;

    omp_set_num_threads(workers);

    boundary = gridSize + 1;
    iterations = iterations * 0.5;

    initiate();

    free(new);
    free(grid);
}