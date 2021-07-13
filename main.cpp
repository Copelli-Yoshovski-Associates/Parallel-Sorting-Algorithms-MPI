#include "bitonic.h"
//#include "quicksort.h"

///////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    size = atoi(argv[1]);
    if (size < defaultSize)
        size = defaultSize;
    bitonic b;
    //quicksort q;
    // Initialization, get # of processes & this PID/rank
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    // Initialize Array for Storing Random Numbers
    arraySize = size / num_processes;
    array = new int[arraySize]; //(int*)malloc(arraySize * sizeof(int));

    // Generate Random Numbers for Sorting (within each process)
    // Less overhead without MASTER sending random numbers to each slave
    srand(time(NULL) + 100 * process_rank); // Needed for rand()
    for (int i = 0; i < arraySize; i++)
        array[i] = rand() % arraySize;

    // Blocks until all processes have finished generating
    MPI_Barrier(MPI_COMM_WORLD);
    b.start();

    MPI_Barrier(MPI_COMM_WORLD);
    //  q.start();

    // Done
    MPI_Finalize();
    return 0;
}