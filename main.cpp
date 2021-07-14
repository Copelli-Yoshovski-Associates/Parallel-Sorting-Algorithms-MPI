#include "bitonic.h"
#include "quicksort.h"

///////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    size = atoi(argv[1]);
    if (size < defaultSize)
        size = defaultSize;
    globalArray = new int[size];
    srand(time(NULL)); // Needed for rand()

    // Generate Random Numbers for Sorting (within each process)
    // Less overhead without MASTER sending random numbers to each slave
    for (int i = 0; i < size; i++)
        globalArray[i] = rand() % size;

    bitonic b;
    quicksort q;
    // Initialization, get # of processes & this PID/rank

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    // Initialize Array for Storing Random Numbers
    arraySize = size / num_processes;
    b.start();

    q.start();

    // Done

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    delete[] globalArray;

    return 0;
}