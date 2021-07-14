#include "bitonic.h"
#include "quicksort.h"

int controllaSize(int oldSize)
{
    auto res = (double)log2(oldSize);

    if (res != (int)res)
        oldSize = pow(2, (int)res + 1);

    return oldSize;
}

///////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    size = atoi(argv[1]);
    /*  if (size < defaultSize)
        size = defaultSize;
    */

    size = controllaSize(size);

    if (size < LIMIT)
        OUTPUT_NUM = size;
    int *globalArray = new int[size];
    srand(time(NULL)); // Needed for rand()

    // Generate Random Numbers for Sorting (within each process)
    // Less overhead without MASTER sending random numbers to each slave
    for (int i = 0; i < size; i++)
        globalArray[i] = rand() % size;

    printArray(globalArray);

    bitonic b;

    quicksort q;
    // Initialization, get # of processes & this PID/rank

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (process_rank == MASTER && num_processes != controllaSize(num_processes))
    {
        printf("Errore! Si prega di utilizzare un numero di processori che sia potenza di 2...\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Initialize Array for Storing Random Numbers
    arraySize = size / num_processes;
    b.start();

    MPI_Barrier(MPI_COMM_WORLD);
    q.start();

    // Done

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    delete[] globalArray;

    return 0;
}