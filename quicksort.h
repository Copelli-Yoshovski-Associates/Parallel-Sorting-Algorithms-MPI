#ifndef QUICKSORT_H
#define QUICKSORT_H
#include "settings.h"
#include <climits>
class quicksort
{
private:
    int local_size = arraySize;

public:
    quicksort() {}
    /*
	getRandomArray(array, size)
	fill array with random integers in the range from 0 to RAND_MAX
*/
    void getRandomArray(int *arr, int s)
    {
        for (int i = 0; i < s; i++)
            arr[i] = rand() % RAND_MAX;
    }

    /*
	merge(array1, array2, merged_array, size)
	merge two arrays with the same size into array
	in a ascending order
*/
    void merge(int *array1, int *array2, int *merged_array, int s)
    {
        int i = 0, j = 0, k = 0;
        while (i < s / 2 && j < s / 2)
        {
            merged_array[k++] = array1[i] < array2[j] ? array1[i++] : array2[j++];
        }
        while (i < s / 2)
        {
            merged_array[k++] = array1[i++];
        }
        while (j < s / 2)
        {
            merged_array[k++] = array2[j++];
        }
    }

    /*
	partition(array, int, size)
	divide the array into two parts (elements <= pivot and elements > pivot)
	obtained from AUCSC 310 lecture slides
*/
    int partition(int *arr, int start, int end)
    {
        int pivot = arr[end];
        int smallerCount = start;
        for (int j = start; j < end; j++)

            if (arr[j] <= pivot)
            {
                int temp = arr[j];
                arr[j] = arr[smallerCount];
                arr[smallerCount] = temp;
                smallerCount += 1;
            }

        arr[end] = arr[smallerCount];
        arr[smallerCount] = pivot;
        return smallerCount;
    }

    /*
	quickSort(array, start, end)
	call partition to divide array into two parts - divide
	then recursively call itself on the two parts - conquer
*/
    void quickSort(int *arr, int start, int end)
    {
        if (start < end)
        {
            int pivotLoc = partition(arr, start, end);
            quickSort(arr, start, pivotLoc - 1);
            quickSort(arr, pivotLoc + 1, end);
        }
    }

    /*
	quickSort(array, size)
	a wrapper for quickSort(array, start, end)
*/
    void quickSort(int *arr, int s)
    {
        quickSort(arr, 0, s - 1);
    }

    void printArray(int *A, int s)
    {
        for (int i = 0; i < s; i++)

            printf("A[%d] is %d\n", i, A[i]);

        printf("\n");
    }

    void start()
    {
        int *local_A = new int[local_size]; // (int *)malloc(local_size * sizeof(int));
        //int *local_A = array;
        //performance(run time) testing
        MPI_Barrier(MPI_COMM_WORLD);
        if (process_rank == MASTER)
        {
            //generate an array of random integers
            // A = new int[size]; //(int *)malloc(size * sizeof(int));
            //  getRandomArray(A, size);
            //printArray(local_A, local_size);
            timer_start = MPI_Wtime();
        }
        //distribute A to local_A for each process
        MPI_Scatter(globalArray, local_size, MPI_INT, local_A, local_size, MPI_INT, 0, MPI_COMM_WORLD);

        //printArray(local_A, local_size);
        quickSort(local_A, local_size); //perform local quicksort

        //merge tree (see details in report)
        int merge_tree_depth = (int)log2(num_processes);
        int my_index = process_rank; //used to determine each process's behavior in merge part
        int step = 0;
        int *current_array = local_A;
        while (step < merge_tree_depth)
        { //depth of merge tree
            int merged_size = local_size * (int)pow(2, step + 1);
            if (my_index % 2 == 0)
            {

                int *merged_array = new int[merged_size];   // (int *)malloc(merged_size * sizeof(int));
                int *recv_array = new int[merged_size / 2]; //(int *)malloc((merged_size / 2) * sizeof(int));

                int src_rank = process_rank + (int)pow(2, step);
                MPI_Recv(recv_array, merged_size / 2, MPI_INT, src_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                merge(current_array, recv_array, merged_array, merged_size);
                current_array = merged_array;
                //printArray(current_array, merged_size);
                delete[] recv_array;
                delete[] merged_array;
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

        if (process_rank == 0)
        {
            //counting ends
            timer_end = MPI_Wtime();
            //printArray(current_array, size);
            //process 0 is the last one that ends while loop
            //printArray(current_array, size);
            std::cout << "With comm_sz = " << num_processes << " and input array size = " << size
                      << ", elapsed time is " << timer_end - timer_start << " seconds\n";
        }
        /*  delete[] A;*/
    }
};

#endif