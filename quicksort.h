#ifndef QUICKSORT_H
#define QUICKSORT_H
#include "settings.h"

class quicksort
{
private:
public:
    quicksort();
    /*
	merge(array1, array2, merged_array, size)
	merge two arrays with the same size into array
	in a ascending order
*/
    void merge(int *array1, int *array2, int *merged_array, int size)
    {
        int i = 0, j = 0, k = 0;
        while (i < size / 2 && j < size / 2)
        {
            merged_array[k++] = array1[i] < array2[j] ? array1[i++] : array2[j++];
        }
        while (i < size / 2)
        {
            merged_array[k++] = array1[i++];
        }
        while (j < size / 2)
        {
            merged_array[k++] = array2[j++];
        }
    }
    /*
	getRandomArray(array, size)
	fill array with random integers in the range from 0 to RAND_MAX
*/
    void getRandomArray(int *array, int size)
    {
        int i = 0;
        while (i < size)
        {
            array[i] = rand() % size;
            i++;
        }
    }

    /*
	partition(array, int, size)
	divide the array into two parts (elements <= pivot and elements > pivot)
	obtained from AUCSC 310 lecture slides
*/
    int partition(int *array, int start, int end)
    {
        if (showGraphic)
        {
            showGraphics(array);
            //MPI_Barrier(MPI_COMM_WORLD);
        }
        int pivot = array[end];
        int smallerCount = start;
        for (int j = start; j < end; j++)
        {
            if (array[j] <= pivot)
            {
                int temp = array[j];
                array[j] = array[smallerCount];
                array[smallerCount] = temp;
                smallerCount += 1;
            }
        }
        array[end] = array[smallerCount];
        array[smallerCount] = pivot;
        return smallerCount;
    }

    /*
	quickSort(array, start, end)
	call partition to divide array into two parts - divide
	then recursively call itself on the two parts - conquer
*/
    void quickSort(int *array, int start, int end)
    {
        // MPI_Barrier(MPI_COMM_WORLD);
        if (start < end)
        {
            int pivotLoc = partition(array, start, end);
            quickSort(array, start, pivotLoc - 1);
            quickSort(array, pivotLoc + 1, end);
        }
    }

    /*
	quickSort(array, size)
	a wrapper for quickSort(array, start, end)
*/
    void quickSort(int *array, int size)
    {
        quickSort(array, 0, size - 1);
    }

    void start();
};

quicksort::quicksort(/* args */)
{
}

void quicksort::start()
{

    int *A = NULL;                     //the array to sort
    int *local_A = new int[arraySize]; //(int *)malloc(arraySize * sizeof(int));
    if (process_rank == MASTER)
    {
        //generate an array of random integers
        /*  A = new int[size]; //(int *)malloc(size * sizeof(int));
        getRandomArray(A, size);
        //printArray(A, size);*/

        leggiNumeriRandom(A);
    }

    //performance(run time) testing
    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
        timer_start = MPI_Wtime();

    //distribute A to local_A for each process
    MPI_Scatter(A, arraySize, MPI_INT, local_A, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

    quickSort(local_A, arraySize); //perform local quicksort
    MPI_Barrier(MPI_COMM_WORLD);
    //merge tree (see details in report)
    int merge_tree_depth = (int)log2(num_processes);
    int my_index = process_rank; //used to determine each process's behavior in merge part
    int step = 0;
    int *current_array = local_A;
    while (step < merge_tree_depth)
    { //depth of merge tree
        int merged_size = arraySize * (int)pow(2, step + 1);
        if (my_index % 2 == 0)
        {

            int *merged_array = new int[merged_size];   //(int *)malloc(merged_size * sizeof(int));
            int *recv_array = new int[merged_size / 2]; // (int *)malloc((merged_size / 2) * sizeof(int));

            int src_rank = process_rank + (int)pow(2, step);
            MPI_Recv(recv_array, merged_size / 2, MPI_INT, src_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            merge(current_array, recv_array, merged_array, merged_size);
            current_array = merged_array;
            //printArray(current_array, merged_size);
            delete[] recv_array;
        }
        else
        {
            int des_rank = process_rank - (int)pow(2, step);
            MPI_Send(current_array, merged_size / 2, MPI_INT, des_rank, 0, MPI_COMM_WORLD);
            break;
        }
        step++;
        my_index = my_index / 2;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (process_rank == MASTER)
    {
        //counting ends
        timer_end = MPI_Wtime();

        printInfo(2, current_array);

        if (DEBUG)
            stampaArrayOrdinato(current_array);
        //process 0 is the last one that ends while loop
        //printArray(current_array, size);
        printTime();
    }
    MPI_Barrier(MPI_COMM_WORLD);
    delete[] current_array;
    delete[] A;
}

#endif