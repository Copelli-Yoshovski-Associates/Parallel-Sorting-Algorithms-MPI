#ifndef BITONIC_H
#define BITONIC_H
#include "settings.h"

int compareBitonic(const void *a, const void *b)
{
    if (*(int *)a > *(int *)b)
        return 1;
    else if (*(int *)a < *(int *)b)
        return -1;
    return 0;
}

class bitonic
{
private:
    int *Print;
    unsigned andBit;
    enum
    {
        HIGH = 0,
        LOW
    };

    int *local_list, *temp_list, *merge_list;

    void mergeSplit(int arraySize, int *local_list, int which_keys, int partner, MPI_Comm comm)
    {
        MPI_Status status;
        temp_list = new int[arraySize]; //(int *)malloc(arraySize * sizeof(int));
        MPI_Sendrecv(local_list, arraySize, MPI_INT, partner, 0, temp_list, arraySize, MPI_INT, partner, 0, comm, &status);
        if (which_keys == HIGH)
            mergeHigh(arraySize, local_list, temp_list);
        else
            mergeLow(arraySize, local_list, temp_list);
        //free(temp_list);
        delete[] temp_list;
    }

    void bitonicsort_increase(int arraySize, int *local_list, int dimProcessori, MPI_Comm comm)
    {
        unsigned eor_bit;
        int processorDimension, stage, partner;

        processorDimension = log2(dimProcessori);
        eor_bit = 1 << (processorDimension - 1);
        for (stage = 0; stage < processorDimension; stage++)
        {
            partner = process_rank ^ eor_bit;
            if (process_rank < partner)
                mergeSplit(arraySize, local_list, LOW, partner, comm);
            else
                mergeSplit(arraySize, local_list, HIGH, partner, comm);
            eor_bit = eor_bit >> 1;
        }
    }

    void mergeLow(int arraySize, int *list1, int *list2)
    {
        int i;
        int index1 = 0;
        int index2 = 0;
        merge_list = new int[arraySize]; //(int *)malloc(arraySize * sizeof(int));
        for (i = 0; i < arraySize; i++)
            if (list1[index1] <= list2[index2])
            {
                merge_list[i] = list1[index1];
                index1++;
            }
            else
            {
                merge_list[i] = list2[index2];
                index2++;
            }

        for (i = 0; i < arraySize; i++)
            list1[i] = merge_list[i];
        // free(merge_list);
        delete[] merge_list;
    }

    void mergeHigh(int arraySize, int *list1, int *list2)
    {
        int i;
        int index1 = arraySize - 1;
        int index2 = arraySize - 1;
        merge_list = new int[arraySize]; //(int *)malloc(arraySize * sizeof(int));
        for (i = arraySize - 1; i >= 0; i--)
            if (list1[index1] >= list2[index2])
            {
                merge_list[i] = list1[index1];
                index1--;
            }
            else
            {
                merge_list[i] = list2[index2];
                index2--;
            }

        for (i = 0; i < arraySize; i++)
            list1[i] = merge_list[i];
        // free(merge_list);
        delete[] merge_list;
    }

    void bitonicsort_decrease(int arraySize, int *local_list, int dimProcessori, MPI_Comm comm)
    {
        unsigned eor_bit;
        int processorDimension, stage, partner;

        processorDimension = log2(dimProcessori);
        eor_bit = 1 << (processorDimension - 1);
        for (stage = 0; stage < processorDimension; stage++)
        {
            partner = process_rank ^ eor_bit;
            if (process_rank > partner)
                mergeSplit(arraySize, local_list, LOW, partner, comm);
            else
                mergeSplit(arraySize, local_list, HIGH, partner, comm);
            eor_bit = eor_bit >> 1;
        }
    }

public:
    bitonic() {}
    void start();
};

void bitonic::start()
{
    int *array;

    local_list = new int[arraySize]; //(int *)malloc(arraySize * sizeof(int));

    if (process_rank == MASTER)
        readFromFile(array);

    MPI_Scatter(array, arraySize, MPI_INT, local_list, arraySize, MPI_INT, MASTER, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    if (DEBUG)
    {

        printf("Processor %d: ", process_rank);
        for (int i = 0; i < arraySize; i++)
            printf("%d ", local_list[i]);
        printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
        timer_start = MPI_Wtime();
    qsort(local_list, arraySize, sizeof(int), compareBitonic);

    for (int dimProcessori = 2, andBit = 2; dimProcessori <= num_processes; dimProcessori = dimProcessori * 2, andBit = andBit << 1)
        if ((process_rank & andBit) == 0)
            bitonicsort_increase(arraySize, local_list, dimProcessori, MPI_COMM_WORLD);
        else
            bitonicsort_decrease(arraySize, local_list, dimProcessori, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if (process_rank == MASTER)
    {
        timer_end = MPI_Wtime();
        Print = new int[size]; //(int *)malloc(size * sizeof(int));
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(local_list, arraySize, MPI_INT, Print, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

    if (process_rank == MASTER)
    {
        if (DEBUG)
            stampaArrayOrdinato(Print);

        printInfo(1, Print);
        printTime();
        delete[] Print;
        delete[] local_list;
    }
}

#endif
