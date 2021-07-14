#ifndef SETTINGS_H
#define SETTINGS_H

#define MASTER 0      // Who should do the final processing
#define OUTPUT_NUM 10 // Number of elements to display in output
#include <iostream>   // Printf
#include <time.h>     // Timer
#include <math.h>     // Logarithm
#include <mpi.h>      // MPI Library

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

#endif