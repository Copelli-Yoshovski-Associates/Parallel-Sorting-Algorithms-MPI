#ifndef SETTINGS_H
#define SETTINGS_H

#define MASTER 0     // Who should do the final processing
int OUTPUT_NUM = 10; // Number of elements to display in output
#include <iostream>  // Printf
#include <time.h>    // Timer
#include <math.h>    // Logarithm
#include <mpi.h>     // MPI Library
#include "allegro5/allegro.h"
#include "allegro5/allegro_primitives.h"
#include "allegro5/allegro_image.h"
//#include <windows.h>
#include <stdio.h>

#include <fstream>
#include <ios>
#define LIMIT 10
#define DEBUG 0

#define WINDOWSIZE 600
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_COLOR color = al_map_rgb(255, 0, 0);
int scala = WINDOWSIZE / 2;
const char *nomeFile = "Timing.txt";
bool showGraphic = false;

// Globals
double timer_start;
double timer_end;
int process_rank;
int num_processes;

//int *globalArray;
int size;
int arraySize;
int defaultSize = 100;

double global_elapsed_time;

void printTime()
{
    printf("Eseguito con %d processori, dimensione array pari a %d, tempo impiegato: %f secondi\n\n", num_processes, size, timer_end - timer_start);
}

std::string determineSort(int sortType)
{
    std::string sortName = "Undefined";
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
        break;
    }
    return sortName;
}

void printInfo(int sortType, int *Print)
{

    std::string sortName = determineSort(sortType);
    printf("Displaying sorted array (only %d elements for quick verification of ", OUTPUT_NUM);
    printf(determineSort(sortType).append(")\n").c_str());

    // Print Sorting Results
    for (int i = 0; i < size; i++)
        if ((i % (size / OUTPUT_NUM)) == 0)
            printf("%d ", Print[i]);

    printf("\n");

    /*   FILE *f = fopen(nomeFile, "a");
    fprintf(f, sortName.c_str());
    fprintf(f, ": %d %d %f\n", size, num_processes, timer_end - timer_start);*/

    std::ofstream fout(nomeFile, std::ios::app);
    fout << sortName << ": " << size << " " << num_processes << " " << (timer_end - timer_start) << std::endl;
    fout.close();
}

void stampaArrayOrdinato(int *Print)
{
    printf("Dimensione Array: %d\n", size);
    printf("Numero Processori usati: %d\n", num_processes);
    printf("Tempo (in sec): %f\n", timer_end - timer_start);

    printf("Array Ordinato: ");

    for (int i = 0; i < size; i++)
        printf("%d ", Print[i]);
    printf("\n");
}

void showGraphics(const int *local_A)
{
    if (display == NULL)
        return;
    if (process_rank == MASTER)
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        int salto = 1;
        int div = 1;
        int filler = 2;
        if (size > scala)
        {
            salto = (arraySize / scala) + 1;
            div = size / scala;
        }
        else
            filler = WINDOWSIZE / arraySize;

        unsigned posizioneX = 0;
        for (int i = 0; i < arraySize; i += salto)
        {

            int val = WINDOWSIZE - (local_A[i] / div);

            al_draw_line(posizioneX, WINDOWSIZE, posizioneX, val, color, 1.0);
            posizioneX += filler;
        }
        al_flip_display();
        al_rest(0.2);
    }
}

void printArray(int arr[])
{
    std::ofstream fout("input.txt");
    fout << size << std::endl;
    for (int i = 0; i < size; i++)
        fout << arr[i] << " ";
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