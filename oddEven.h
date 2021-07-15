#ifndef ODDEVEN_H
#define ODDEVEN_H

#include "settings.h"

int compare(const void *a, const void *b)
{
    if (*(int *)a > *(int *)b)
        return 1;
    else if (*(int *)a < *(int *)b)
        return -1;

    return 0;
}

class oddEven
{
private:
    enum
    {
        MAX = 0,
        MIN
    };

    int evenPartner, oddPartner, upperI;
    MPI_Status status;
    int *tmpMergeListA, *tmpMergeListB, *x;

public:
    oddEven();
    ~oddEven();
    void start();

    void merge(int tipo)
    {
        int a, b, c;
        if (tipo == MAX)
        {
            a = b = c = arraySize - 1;
            while (c >= 0)
            {
                if (x[a] >= tmpMergeListA[b])
                    tmpMergeListB[c--] = x[a--];
                else
                    tmpMergeListB[c--] = tmpMergeListA[b--];
            }
        }
        else if (tipo == MIN)
        {
            a = b = c = 0;
            while (c < arraySize)
            {
                if (x[a] <= tmpMergeListA[b])
                    tmpMergeListB[c++] = x[a++];
                else
                    tmpMergeListB[c++] = tmpMergeListA[b++];
            }
        }
        memcpy(x, tmpMergeListB, arraySize);
    }

    void oddEvenTranspose(int fase)
    {
        MPI_Status status;
        if ((fase % 2) == 0)
        {
            /*even phase */
            if (evenPartner >= 0)
            {
                MPI_Sendrecv(x, arraySize, MPI_INT, evenPartner, 0, tmpMergeListA, arraySize, MPI_INT, evenPartner, 0, MPI_COMM_WORLD, &status);
                if ((process_rank % 2) == 0)
                    /*extract highest or lowest I values from merged list*/
                    merge(MIN);
                else
                    merge(MAX);
            }
        }
        /*odd fase */
        if (oddPartner >= 0)
        {
            MPI_Sendrecv(x, arraySize, MPI_INT, oddPartner, 0, tmpMergeListA, arraySize, MPI_INT, oddPartner, 0, MPI_COMM_WORLD, &status);
            if ((process_rank % 2) == 0)
                merge(MAX);
            else
                merge(MIN);
        }
    }
};

oddEven::oddEven(/* args */)
{
}

oddEven::~oddEven()
{
}

void oddEven::start()
{
    if (process_rank == MASTER)
    {
        readFromFile(x);
        printf("OddEven partito\nArray letto da input: ");
        stampaArrayOrdinato(x);
    } /*
    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
        timer_start = MPI_Wtime();
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

    qsort(x, arraySize, sizeof(int), compare);

    tmpMergeListA = new int[arraySize];
    tmpMergeListB = new int[arraySize];

    for (int fase = 0; fase < num_processes; fase++)
        oddEvenTranspose(fase);

    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
        timer_end = MPI_Wtime();
    
    int *Print = NULL;
    if (process_rank == MASTER)
    {
        Print = new int[size];
        MPI_Gather(x, arraySize, MPI_INT, Print, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

        printInfo(3, Print);

        if (DEBUG)
            stampaArrayOrdinato(Print);
        printTime();
    }
    else
        MPI_Gather(x, arraySize, MPI_INT, Print, arraySize, MPI_INT, 0, MPI_COMM_WORLD);
*/
    MPI_Barrier(MPI_COMM_WORLD);
    // if (process_rank == MASTER)
    delete[] x;
    /*delete[] tmpMergeListA;
    delete[] tmpMergeListB;
    delete[] Print;*/
}

#endif