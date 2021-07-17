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
    void sorting(int *arr, int n)
    {
        bool sorted = false;
        while (!sorted)
        {

            if (showGraphic)
            {
                //MPI_Barrier(MPI_COMM_WORLD);
                showGraphics(arr);
            }

            sorted = true;
            for (int i = 1; i < n - 1; i += 2)
            {
                if (arr[i] > arr[i + 1])
                {
                    int temp = arr[i];
                    arr[i] = arr[i + 1];
                    arr[i + 1] = temp;
                    sorted = false;
                }
            }

            for (int i = 0; i < n - 1; i += 2)
            {
                if (arr[i] > arr[i + 1])
                {
                    int temp = arr[i];
                    arr[i] = arr[i + 1];
                    arr[i + 1] = temp;
                    sorted = false;
                }
            }
        }
    }

    void mergeMax(int x[], int tmpMergeListA[], int tmpMergeListB[], int I)
    {
        /*mergesort a and b */
        /*return n highest values */
        int a, b, c;
        a = b = c = I - 1;
        while (c >= 0)
        {
            if (x[a] >= tmpMergeListA[b])
                tmpMergeListB[c--] = x[a--];
            else
                tmpMergeListB[c--] = tmpMergeListA[b--];
        }

        memcpy(x, tmpMergeListB, I * sizeof(int));
    }

    void mergeMin(int x[], int tmpMergeListA[], int tmpMergeListB[], int I)
    {
        /*mergesort a and b */
        /*return n lowest values */
        int a, b, c;
        a = b = c = 0;
        while (c < I)
        {
            if (x[a] <= tmpMergeListA[b])
                tmpMergeListB[c++] = x[a++];
            else
                tmpMergeListB[c++] = tmpMergeListA[b++];
        }
        memcpy(x, tmpMergeListB, I * sizeof(int));
    }
    void oddEvenTranspose(int x[], int tmpMergeListA[], int tmpMergeListB[], int I, int phase, int evenPartner, int oddPartner, int evenPartnerI, int oddPartnerI, int myrank, int P, MPI_Comm comm)
    {

        MPI_Status status;
        if ((phase % 2) == 0)
        {
            /*even phase */
            if (evenPartner >= 0)
            {
                MPI_Sendrecv(x, I, MPI_INT, evenPartner, 0,
                             tmpMergeListA, I, MPI_INT, evenPartner, 0, comm, &status);
                if ((myrank % 2) == 0)
                {
                    /*extract highest or lowest I values from merged list*/
                    mergeMin(x, tmpMergeListA, tmpMergeListB, I);
                }
                else
                {
                    mergeMax(x, tmpMergeListA, tmpMergeListB, I);
                }
            }
        }
        else
        {
            /*odd phase */
            if (oddPartner >= 0)
            {
                MPI_Sendrecv(x, I, MPI_INT, oddPartner, 0,
                             tmpMergeListA, I, MPI_INT, oddPartner, 0, comm, &status);
                if ((myrank % 2) == 0)
                {
                    mergeMax(x, tmpMergeListA, tmpMergeListB, I);
                }
                else
                {
                    mergeMin(x, tmpMergeListA, tmpMergeListB, I);
                }
            }
        }
    }

    int evenPartner, oddPartner;
    int *tmpMergeListA, *tmpMergeListB;
    int evenPartnerI, oddPartnerI, myrank, P;

public:
    oddEven();
    void start();
};

oddEven::oddEven(/* args */)
{
}

void oddEven::start()
{
    myrank = process_rank;
    P = num_processes;
    int *x = NULL;
    int *local_A = new int[arraySize];
    if (process_rank == MASTER)
        leggiNumeriRandom(x);

    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
        timer_start = MPI_Wtime();
    MPI_Scatter(x, arraySize, MPI_INT, local_A, arraySize, MPI_INT, 0, MPI_COMM_WORLD);
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
    // qsort(local_A, arraySize, sizeof(int), compare);
    sorting(local_A, arraySize);
    MPI_Barrier(MPI_COMM_WORLD);
    tmpMergeListA = new int[arraySize];
    tmpMergeListB = new int[arraySize];

    for (int fase = 0; fase < num_processes; fase++)
        //  for (int fase = 0; fase < arraySize-; fase++)
        oddEvenTranspose(local_A, tmpMergeListA, tmpMergeListB,
                         arraySize, fase, evenPartner, oddPartner,
                         evenPartnerI, oddPartnerI, myrank, P,
                         MPI_COMM_WORLD);

    //qsort(local_A, arraySize, sizeof(int), compare);

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