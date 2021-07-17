#ifndef ODDEVEN_H
#define ODDEVEN_H
#include "settings.h"

class oddEven
{
private:
    void sorting(int *array, int dim)
    {
        bool sorted = false;
        while (!sorted)
        {
            if (showGraphic)
                showGraphics(array);

            sorted = true;
            for (int j = 0; j < 2; j++)
                for (int i = 1 - j; i < dim - 1; i += 2)
                    if (array[i] > array[i + 1])
                    {
                        int temp = array[i];
                        array[i] = array[i + 1];
                        array[i + 1] = temp;
                        sorted = false;
                    }
        }
    }
    //Wrapper unisciMin/unisciMax
    void unisciMin(int *x, int *listaTemp1, int *listaTemp2, int dim) { unisci(x, listaTemp1, listaTemp2, dim, false); }
    void unisciMax(int *x, int *listaTemp1, int *listaTemp2, int dim) { unisci(x, listaTemp1, listaTemp2, dim, true); }

    void unisci(int *x, int *listaTemp1, int *listaTemp2, int dim, bool max)
    {
        int j = 0, k = 0, z = 0;
        if (max)
        {
            j = dim - 1, k = dim - 1, z = dim - 1;
            while (z >= 0)
            {
                if (x[j] >= listaTemp1[k])
                    listaTemp2[z--] = x[j--];
                else
                    listaTemp2[z--] = listaTemp1[k--];
            }
        }
        else
        {
            while (z < dim)
            {
                if (x[j] <= listaTemp1[k])
                    listaTemp2[z++] = x[j++];
                else
                    listaTemp2[z++] = listaTemp1[k++];
            }
        }
        for (int i = 0; i < dim; i++)
            x[i] = listaTemp2[i];
    }
    void TrasposizioneOddEven(int *x, int *listaTemp1, int *listaTemp2, int dim, int fase, int vicinoPari, int vicinoDispari)
    {
        MPI_Status status;
        if (fase % 2 != 0)
        {

            if (vicinoDispari >= 0)
            {
                MPI_Sendrecv(x, dim, MPI_INT, vicinoDispari, 0, listaTemp1, dim, MPI_INT, vicinoDispari, 0, MPI_COMM_WORLD, &status);
                if ((process_rank % 2) == 0)
                    unisciMax(x, listaTemp1, listaTemp2, dim);
                else
                    unisciMin(x, listaTemp1, listaTemp2, dim);
            }
        }
        else
        {
            if (vicinoPari >= 0)
            {
                MPI_Sendrecv(x, dim, MPI_INT, vicinoPari, 0, listaTemp1, dim, MPI_INT, vicinoPari, 0, MPI_COMM_WORLD, &status);
                if ((process_rank % 2) == 0)
                    unisciMin(x, listaTemp1, listaTemp2, dim);
                else
                    unisciMax(x, listaTemp1, listaTemp2, dim);
            }
        }
    }

    int vicinoPari, vicinoDispari, *listaTemp1, *listaTemp2;

public:
    void start()
    {
        int *x = NULL;
        int *arrayLocale = new int[arraySize];
        if (process_rank == MASTER)
        {
            x = globalArray;
            //leggiNumeriRandom(x);
            timer_start = MPI_Wtime();
        }

        if (process_rank % 2 != 0)
        {
            vicinoPari = process_rank - 1;
            vicinoDispari = (process_rank + 1);

            if (vicinoDispari == num_processes)
                vicinoDispari = -1;
        }
        else
        {
            vicinoPari = (process_rank + 1);
            if (vicinoPari == num_processes)
                vicinoPari = -1;
            vicinoDispari = process_rank - 1;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Scatter(x, arraySize, MPI_INT, arrayLocale, arraySize, MPI_INT, 0, MPI_COMM_WORLD);

        sorting(arrayLocale, arraySize);

        listaTemp1 = new int[arraySize];
        listaTemp2 = new int[arraySize];

        for (int passo = 0; passo < num_processes; passo++)
            TrasposizioneOddEven(arrayLocale, listaTemp1, listaTemp2, arraySize, passo, vicinoPari, vicinoDispari);

        MPI_Barrier(MPI_COMM_WORLD);
        if (process_rank == MASTER)
            timer_end = MPI_Wtime();

        delete[] listaTemp1;
        delete[] listaTemp2;
        int *Print = NULL;
        MPI_Barrier(MPI_COMM_WORLD);
        if (process_rank == MASTER)
            Print = new int[size];
        MPI_Gather(arrayLocale, arraySize, MPI_INT, Print, arraySize, MPI_INT, MASTER, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        if (process_rank == MASTER)
        {
            if (DEBUG)
                stampaArrayOrdinato(Print);
            printInfo(3, Print);
            printTime();
            delete[] Print;
        }
        MPI_Barrier(MPI_COMM_WORLD);

        delete[] arrayLocale;
    }
};
#endif