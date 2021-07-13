#ifndef BITONIC_H
#define BITONIC_H
#include "settings.h"

int ComparisonFunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

class bitonic
{
private:
    int *localArray;

public:
    bitonic() {}
    void start();
    ~bitonic();
    void SequentialSort(void);

    void Comparator(int j, bool expression);
};

void bitonic::Comparator(int j, bool expression)
{
    if (!expression)
    {
        int i, max;

        // Receive max from L Process's entire array
        int recv_counter;
        int *buffer_recieve = new int[arraySize]; // (int*)(malloc((arraySize + 1) * sizeof(int)));
        MPI_Recv(
            &max,                    // buffer max value
            1,                       // one item
            MPI_INT,                 // INT
            process_rank ^ (1 << j), // paired process calc by XOR with 1 shifted left j positions
            0,                       // tag 0
            MPI_COMM_WORLD,          // default comm.
            MPI_STATUS_IGNORE        // ignore info about message received
        );

        // Send min to L Process of current process's array
        int send_counter = 0;
        int *buffer_send = new int[arraySize]; //(int*)(malloc((arraySize + 1) * sizeof(int)));
        MPI_Send(
            &localArray[0],          // send min
            1,                       // one item
            MPI_INT,                 // INT
            process_rank ^ (1 << j), // paired process calc by XOR with 1 shifted left j positions
            0,                       // tag 0
            MPI_COMM_WORLD           // default comm.
        );

        // Buffer a list of values which are smaller than max value
        for (i = 0; i < arraySize; i++)
        {
            if (localArray[i] < max)
            {
                buffer_send[send_counter + 1] = localArray[i];
                send_counter++;
            }
            else
            {
                break; // Important! Saves lots of cycles!
            }
        }

        // Receive blocks greater than min from paired slave
        MPI_Recv(
            buffer_recieve,          // buffer message
            arraySize,               // whole array
            MPI_INT,                 // INT
            process_rank ^ (1 << j), // paired process calc by XOR with 1 shifted left j positions
            0,                       // tag 0
            MPI_COMM_WORLD,          // default comm.
            MPI_STATUS_IGNORE        // ignore info about message receiveds
        );
        recv_counter = buffer_recieve[0];

        // send partition to paired slave
        buffer_send[0] = send_counter;
        MPI_Send(
            buffer_send,             // all items smaller than max value
            send_counter,            // # of values smaller than max
            MPI_INT,                 // INT
            process_rank ^ (1 << j), // paired process calc by XOR with 1 shifted left j positions
            0,                       // tag 0
            MPI_COMM_WORLD           // default comm.
        );

        // Take received buffer of values from L Process which are greater than current min
        for (i = 1; i < recv_counter + 1; i++)
        {
            if (buffer_recieve[i] > localArray[0])
            {
                // Store value from message
                localArray[0] = buffer_recieve[i];
            }
            else
            {
                break; // Important! Saves lots of cycles!
            }
        }

        // Sequential Sort
        qsort(localArray, arraySize, sizeof(int), ComparisonFunc);

        // Reset the state of the heap from Malloc
        delete[] buffer_send;
        delete[] buffer_recieve;
        // free(buffer_send);
        //free(buffer_recieve);
    }
    else
    {
        int i, min; /* Sends the biggest of the list and receive the smallest of the list */

        // Send entire array to paired H Process
        // Exchange with a neighbor whose (d-bit binary) processor number differs only at the jth bit.
        int send_counter = 0;
        int *buffer_send = new int[arraySize]; //(int*)(malloc((arraySize + 1) * sizeof(int)));
        MPI_Send(
            &localArray[arraySize - 1], // entire array
            1,                          // one data item
            MPI_INT,                    // INT
            process_rank ^ (1 << j),    // paired process calc by XOR with 1 shifted left j positions
            0,                          // tag 0
            MPI_COMM_WORLD              // default comm.
        );

        // Receive new min of sorted numbers
        int recv_counter;
        int *buffer_recieve = new int[arraySize]; //(int*)(malloc((arraySize + 1) * sizeof(int)));
        MPI_Recv(
            &min,                    // buffer the message
            1,                       // one data item
            MPI_INT,                 // INT
            process_rank ^ (1 << j), // paired process calc by XOR with 1 shifted left j positions
            0,                       // tag 0
            MPI_COMM_WORLD,          // default comm.
            MPI_STATUS_IGNORE        // ignore info about message received
        );

        // Buffers all values which are greater than min send from H Process.
        for (i = 0; i < arraySize; i++)
        {
            if (localArray[i] > min)
            {
                buffer_send[send_counter + 1] = localArray[i];
                send_counter++;
            }
            else
            {
                break; // Important! Saves lots of cycles!
            }
        }

        buffer_send[0] = send_counter;

        // send partition to paired H process
        MPI_Send(
            buffer_send,             // Send values that are greater than min
            send_counter,            // # of items sent
            MPI_INT,                 // INT
            process_rank ^ (1 << j), // paired process calc by XOR with 1 shifted left j positions
            0,                       // tag 0
            MPI_COMM_WORLD           // default comm.
        );

        // receive info from paired H process
        MPI_Recv(
            buffer_recieve,          // buffer the message
            arraySize,               // whole array
            MPI_INT,                 // INT
            process_rank ^ (1 << j), // paired process calc by XOR with 1 shifted left j positions
            0,                       // tag 0
            MPI_COMM_WORLD,          // default comm.
            MPI_STATUS_IGNORE        // ignore info about message received
        );

        // Take received buffer of values from H Process which are smaller than current max
        for (i = 1; i < buffer_recieve[0] + 1; i++)
        {
            if (localArray[arraySize - 1] < buffer_recieve[i])
            {
                // Store value from message
                localArray[arraySize - 1] = buffer_recieve[i];
            }
            else
            {
                break; // Important! Saves lots of cycles!
            }
        }

        // Sequential Sort
        qsort(localArray, arraySize, sizeof(int), ComparisonFunc);

        // Reset the state of the heap from Malloc
        delete[] buffer_send;
        delete[] buffer_recieve;
        //  free(buffer_send);
        //free(buffer_recieve);
    }
}

void bitonic::start()
{

    this->localArray = new int[arraySize];
    for (int i = 0; i < arraySize; i++)
        this->localArray[i] = array[i];

    int i, j;

    // Begin Parallel Bitonic Sort Algorithm from Assignment Supplement

    // Cube Dimension
    int dimensions = (int)(log2(num_processes));

    // Start Timer before starting first sort operation (first iteration)
    if (process_rank == MASTER)
    {
        printf("Number of Processes spawned: %d\n", num_processes);
        timer_start = MPI_Wtime();
    }

    // Sequential Sort
    qsort(localArray, arraySize, sizeof(int), ComparisonFunc);

    // Bitonic Sort follows
    for (i = 0; i < dimensions; i++)

        for (j = i; j >= 0; j--)
        {
            // (window_id is even AND jth bit of process is 0)
            // OR (window_id is odd AND jth bit of process is 1)
            bool expression = ((process_rank >> (i + 1)) % 2 == 0 && (process_rank >> j) % 2 == 0) || ((process_rank >> (i + 1)) % 2 != 0 && (process_rank >> j) % 2 != 0);
            Comparator(j, expression);
        }

    // Blocks until all processes have finished sorting
    MPI_Barrier(MPI_COMM_WORLD);

    if (process_rank == MASTER)
    {
        timer_end = MPI_Wtime();

        printf("Displaying sorted array (only %d out of %d elements for quick verification)\n", OUTPUT_NUM, size);

        // Print Sorting Results
        for (i = 0; i < arraySize; i++)
        {
            if ((i % (arraySize / OUTPUT_NUM)) == 0)

                printf("%d ", localArray[i]);
        }
        printf("\n\n");

        printf("Time Elapsed (Sec): %f\n", timer_end - timer_start);
    }

    // Reset the state of the heap from Malloc
    delete[] localArray;
    // free(array);
}

bitonic::~bitonic()
{
}

#endif
