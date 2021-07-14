#ifndef BITONIC_H
#define BITONIC_H
#include "settings.h"

int compare(const void *a, const void *b)
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
    int list_size, n, i, processorSize, my_rank, p;
    int *Print;
    unsigned andBit;
    enum
    {
        HIGH = 0,
        LOW
    };

    int *local_list, *temp_list, *merge_list;

    void mergeSplit(int list_size, int *local_list, int which_keys, int partner, MPI_Comm comm)
    {
        MPI_Status status;
        temp_list = new int[list_size]; //(int *)malloc(list_size * sizeof(int));
        MPI_Sendrecv(local_list, list_size, MPI_INT, partner, 0, temp_list, list_size, MPI_INT, partner, 0, comm, &status);
        if (which_keys == HIGH)
            mergeHigh(list_size, local_list, temp_list);
        else
            mergeLow(list_size, local_list, temp_list);
        //free(temp_list);
        delete[] temp_list;
    }

    void bitonicsort_increase(int list_size, int *local_list, int processorSize, MPI_Comm comm)
    {
        unsigned eor_bit;
        int processorDimension, stage, partner;

        processorDimension = log2(processorSize);
        eor_bit = 1 << (processorDimension - 1);
        for (stage = 0; stage < processorDimension; stage++)
        {
            partner = my_rank ^ eor_bit;
            if (my_rank < partner)
                mergeSplit(list_size, local_list, LOW, partner, comm);
            else
                mergeSplit(list_size, local_list, HIGH, partner, comm);
            eor_bit = eor_bit >> 1;
        }
    }

    void mergeLow(int list_size, int *list1, int *list2)
    {
        int i;
        int index1 = 0;
        int index2 = 0;
        merge_list = new int[list_size]; //(int *)malloc(list_size * sizeof(int));
        for (i = 0; i < list_size; i++)
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

        for (i = 0; i < list_size; i++)
            list1[i] = merge_list[i];
        // free(merge_list);
        delete[] merge_list;
    }

    void mergeHigh(int list_size, int *list1, int *list2)
    {
        int i;
        int index1 = list_size - 1;
        int index2 = list_size - 1;
        merge_list = new int[list_size]; //(int *)malloc(list_size * sizeof(int));
        for (i = list_size - 1; i >= 0; i--)
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

        for (i = 0; i < list_size; i++)
            list1[i] = merge_list[i];
        // free(merge_list);
        delete[] merge_list;
    }

    void bitonicsort_decrease(int list_size, int *local_list, int processorSize, MPI_Comm comm)
    {
        unsigned eor_bit;
        int processorDimension, stage, partner;

        processorDimension = log2(processorSize);
        eor_bit = 1 << (processorDimension - 1);
        for (stage = 0; stage < processorDimension; stage++)
        {
            partner = my_rank ^ eor_bit;
            if (my_rank > partner)
                mergeSplit(list_size, local_list, LOW, partner, comm);
            else
                mergeSplit(list_size, local_list, HIGH, partner, comm);
            eor_bit = eor_bit >> 1;
        }
    }

public:
    bitonic() {}
    void start();
};

void bitonic::start()
{
    list_size = arraySize;
    n = size;
    my_rank = process_rank;
    p = num_processes;

    local_list = new int[list_size]; //(int *)malloc(list_size * sizeof(int));

    srand(time(NULL) + my_rank);
    for (i = 0; i < list_size; i++)
        local_list[i] = rand() % size;

    MPI_Barrier(MPI_COMM_WORLD);
    if (DEBUG)
    {

        printf("Processor %d:", my_rank);
        for (int i = 0; i < list_size; i++)
            printf("%d ", local_list[i]);
        printf("\n");
    }
    if (my_rank == MASTER)
        timer_start = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    qsort(local_list, list_size, sizeof(int), compare);

    for (processorSize = 2, andBit = 2; processorSize <= p; processorSize = processorSize * 2, andBit = andBit << 1)
        if ((my_rank & andBit) == 0)
            bitonicsort_increase(list_size, local_list, processorSize, MPI_COMM_WORLD);
        else
            bitonicsort_decrease(list_size, local_list, processorSize, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if (my_rank == MASTER)

        timer_end = MPI_Wtime();

    if (my_rank == MASTER)
        Print = new int[n]; //(int *)malloc(n * sizeof(int));
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(local_list, list_size, MPI_INT, Print, list_size, MPI_INT, 0, MPI_COMM_WORLD);
    if (DEBUG && process_rank == MASTER)
        stampaArrayOrdinato(Print);

    if (my_rank == MASTER)
    {
        printInfo(1, Print);
        delete[] Print;
        delete[] local_list;
    }
}

#endif
