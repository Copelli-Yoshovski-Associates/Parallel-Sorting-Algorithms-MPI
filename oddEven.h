#ifndef ODDEVEN_H
#define ODDEVEN_H
#include "settings.h"

class oddEven
{
private:
    void sorting(int *array, int dim)
    {
        bool sorted = false;
        while (!sorted)
        {
            if (showGraphic)
                showGraphics(array);

            sorted = true;
            for (int j = 0; j < 2; j++)
                for (int i = 1 - j; i < dim - 1; i += 2)
                    if (array[i] > array[i + 1])
                    {
                        int temp = array[i];
                        array[i] = array[i + 1];
                        array[i + 1] = temp;
                        sorted = false;
                    }
        }
    }
    //Wrapper unisciMin/unisciMax
    void unisciMin(int *x, int *tmpMergeListA, int *tmpMergeListB, int dim) { unisci(x, tmpMergeListA, tmpMergeListB, dim, false); }
    void unisciMax(int *x, int *tmpMergeListA, int *tmpMergeListB, int dim) { unisci(x, tmpMergeListA, tmpMergeListB, dim, true); }

    void unisci(int *x, int *tmpMergeListA, int *tmpMergeListB, int dim, bool max)
    {
        int a = 0, b = 0, c = 0;
        if (max)
        {
            a = dim - 1, b = dim - 1, c = dim - 1;
            while (c >= 0)
            {
                if (x[a] >= tmpMergeListA[b])
                    tmpMergeListB[c--] = x[a--];
                else
                    tmpMergeListB[c--] = tmpMergeListA[b--];
            }
        }
        else
        {
            while (c < dim)
            {
                if (x[a] <= tmpMergeListA[b])
                    tmpMergeListB[c++] = x[a++];
                else
                    tmpMergeListB[c++] = tmpMergeListA[b++];
            }
        }
        for (int i = 0; i < dim; i++)
            x[i] = tmpMergeListB[i];
    }
    void oddEvenTranspose(int *x, int *tmpMergeListA, int *tmpMergeListB, int dim, int phase, int evenPartner, int oddPartner)
    {
        MPI_Status status;
        if ((phase % 2) == 0)
        {
            /*even phase */
            if (evenPartner >= 0)
            {
                MPI_Sendrecv(x, dim, MPI_INT, evenPartner, 0, tmpMergeListA, dim, MPI_INT, evenPartner, 0, MPI_COMM_WORLD, &status);
                if ((process_rank % 2) == 0)
                    /*extract highest or lowest I values from merged list*/
                    unisciMin(x, tmpMergeListA, tmpMergeListB, dim);
                else
                    unisciMax(x, tmpMergeListA, tmpMergeListB, dim);
            }
        }
        else
        {

            /*odd phase */
            if (oddPartner >= 0)
            {
                MPI_Sendrecv(x, dim, MPI_INT, oddPartner, 0, tmpMergeListA, dim, MPI_INT, oddPartner, 0, MPI_COMM_WORLD, &status);
                if ((process_rank % 2) == 0)
                    unisciMax(x, tmpMergeListA, tmpMergeListB, dim);
                else
                    unisciMin(x, tmpMergeListA, tmpMergeListB, dim);
            }
        }
    }

    int evenPartner, oddPartner;
    int *tmpMergeListA, *tmpMergeListB;

public:
    oddEven();
    void start();
};

oddEven::oddEven(/* args */)
{
}

void oddEven::start()
{
    int *x = NULL;
    int *local_A = new int[arraySize];
    if (process_rank == MASTER)
    {
        x = globalArray;
        //leggiNumeriRandom(x);
        timer_start = MPI_Wtime();
    }

    if (process_rank % 2 != 0)
    {
        evenPartner = process_rank - 1;
        oddPartner = (process_rank + 1);

        if (oddPartner == num_processes)
            oddPartner = -1;
    }
    else
    {
        evenPartner = (process_rank + 1);
        if (evenPartner == num_processes)
            evenPartner = -1;
        oddPartner = process_rank - 1;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(x, arraySize, MPI_INT, local_A, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

    sorting(local_A, arraySize);

    tmpMergeListA = new int[arraySize];
    tmpMergeListB = new int[arraySize];

    for (int fase = 0; fase < num_processes; fase++)
        oddEvenTranspose(local_A, tmpMergeListA, tmpMergeListB, arraySize, fase, evenPartner, oddPartner);

    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
        timer_end = MPI_Wtime();

    delete[] tmpMergeListA;
    delete[] tmpMergeListB;
    int *Print = NULL;
    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
        Print = new int[size];
    MPI_Gather(local_A, arraySize, MPI_INT, Print, arraySize, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
    {
        printInfo(3, Print);
        //MPI_Gather(local_A, arraySize, MPI_INT, Print, arraySize, MPI_INT, MASTER, MPI_COMM_WORLD);
        if (DEBUG)
            stampaArrayOrdinato(Print);
        printTime();
        delete[] Print;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    delete[] local_A;
}

#endif