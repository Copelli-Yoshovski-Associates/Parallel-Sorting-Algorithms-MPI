#ifndef SETTINGS_H
#define SETTINGS_H

#define MASTER 0     // Who should do the final processing
int OUTPUT_NUM = 10; // Number of elements to display in output
#include <iostream>  // Printf
#include <time.h>    // Timer
#include <math.h>    // Logarithm
#include <mpi.h>     // MPI Library

#include <fstream>
#define LIMIT 10
#define DEBUG 0
const char *nomeFile = "Timing.txt";

// Globals
double timer_start;
double timer_end;
int process_rank;
int num_processes;

int *globalArray;
int size;
int arraySize;
int defaultSize = 100;

double global_elapsed_time;

void printInfo(int sortType, int *Print)
{

    printf("Displaying sorted array (only %d elements for quick verification)\n", OUTPUT_NUM);

    // Print Sorting Results
    for (int i = 0; i < size; i++)
    {
        if ((i % (size / OUTPUT_NUM)) == 0)
        {
            printf("%d ", Print[i]);
        }
    }
    printf("\n\n");

    std::string sortName = "";
    switch (sortType)
    {
    case 1:
        sortName = "Bitonic sort";
        break;
    case 2:
        sortName = "Quick sort";
        break;
    case 3:
        sortName = "Odd Even Transposition sort";
        break;

    default:
        sortName = "Undefined";
        break;
    }

    FILE *f = fopen(nomeFile, "a");
    // fprintf(f, sortName.c_str());
    fprintf(f, ": %d %d %f \n", size, num_processes, timer_end - timer_start);
}

void stampaArrayOrdinato(int *Print)
{
    printf("N: %d\n", size);
    printf("P: %d\n", num_processes);
    printf("Time(sec): %f\n", timer_end - timer_start);

    printf("Final Sorted List: ");

    for (int i = 0; i < size; i++)
        printf("%d ", Print[i]);
    printf("\n");
}

void printArray(int arr[], int n)
{
    std::ofstream fout("input.txt");
    for (int i = 0; i < size; i++)
        fout << globalArray[i] << " ";
    fout << "\n";
    fout.close();
}

void readFromFile(int *&a)
{
    std::ifstream fin("input.txt");
    fin >> size;

    a = new int[size];
    int x;
    for (int i = 0; i < size; i++)
    {
        fin >> x;
        a[i] = x;
    }
    fin.close();
}

#endif