#include "bitonic.h"
#include "quicksort.h"
#include "oddEven.h"

int controllaSize(int oldSize)
{
    /**
    *Controlla che la stringa possa essere distribuita 
    * equamente su tutti i processori
    * Esempio:
    * processori (8), size (125)
    * 125 % 8 --> 5 (ovvero, 5 numeri rimangono non distribuiti
    * sui processori per essere ordinati
    * QUINDI, 8 - 5 --> 3 (dobbiamo incrementare la size di 3, per poter 
    * avere numeri suffcienti per poterli distribuire sui vari processori
    */
    if (oldSize % num_processes != 0)
        oldSize += num_processes - (oldSize % num_processes);
    return oldSize;
}

int controllaProcessori(int nProcessori)
{
    auto res = (double)log2(nProcessori);

    if (res != (int)res)
        nProcessori = pow(2, (int)res + 1);

    return nProcessori;
}

///////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////
int main(int argc, char *argv[])
{

    bitonic b;
    quicksort q;
    oddEven o;

    // Initialization, get # of processes & this PID/rank

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    size = controllaSize(atoi(argv[1]));

    if (size < LIMIT)
        OUTPUT_NUM = size;

    if (process_rank == MASTER)
    {
        if (num_processes != controllaProcessori(num_processes))
        {
            printf("Errore! Si prega di utilizzare un numero di processori che sia potenza di 2...\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        int *globalArray = new int[size];
        srand(time(NULL)); // Needed for rand()
        double start = MPI_Wtime();
        // Generate Random Numbers for Sorting (within each process)
        // Less overhead without MASTER sending random numbers to each slave
        for (int i = 0; i < size; i++)
            globalArray[i] = rand() % size;
        printf("Fine generazione %f\n", MPI_Wtime() - start);
        start = MPI_Wtime();
        printArray(globalArray);
        printf("Fine scrittura su file in %f\n", MPI_Wtime() - start);
        delete[] globalArray;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // Initialize Array for Storing Random Numbers
    arraySize = size / num_processes;
    //b.start();

    MPI_Barrier(MPI_COMM_WORLD);
    //q.start();

    MPI_Barrier(MPI_COMM_WORLD);
    o.start();
    // Done

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
