#ifndef QUICKSORT_H
#define QUICKSORT_H
#include "settings.h"

class quicksort
{
private:
    //Unisce due array con la stessa dimensione con ordinamento crescente
    void unisci(int *array1, int *array2, int *arrayUnito, int size)
    {
        int i = 0, j = 0, k = 0;
        while (i < size / 2 && j < size / 2)
        {
            if (array1[i] < array2[j])
                arrayUnito[k++] = array1[i++];
            else
                arrayUnito[k++] = array2[j++];
        }

        while (i < size / 2)
        {
            arrayUnito[k++] = array1[i++];
        }

        while (j < size / 2)
        {
            arrayUnito[k++] = array2[j++];
        }
    }

    //Divide l'array in due parti (numeri <= numeroPivot e numeri > numeroPivot)
    int partizione(int *array, int inizio, int fine)
    {
        if (showGraphic)
        {
            showGraphics(array);
            //MPI_Barrier(MPI_COMM_WORLD);
        }
        int pivot = array[fine], contatoreMinori = inizio;
        for (int j = inizio; j < fine; j++)
        {
            if (array[j] <= pivot)
            {
                int temp = array[j];
                array[j] = array[contatoreMinori];
                array[contatoreMinori] = temp;
                contatoreMinori++;
            }
        }
        array[fine] = array[contatoreMinori];
        array[contatoreMinori] = pivot;
        return contatoreMinori;
    }
    //Ordina l'array in modo ricorsivo richiamando la partizione per dividere l'array in due parti
    void quickSort(int *array, int inizio, int fine)
    {
        if (inizio < fine)
        {
            int pivotLoc = partizione(array, inizio, fine);
            quickSort(array, inizio, pivotLoc - 1);
            quickSort(array, pivotLoc + 1, fine);
        }
    }
    //Wrapper quickSort
    void quickSort(int *array, int size) { quickSort(array, 0, size - 1); }

public:
    void start()
    {

        int *A = NULL; //array da ordinare che viene letto dal processore MASTER da input
        int *arrayLocale = new int[arraySize];
        if (process_rank == MASTER)
        {
            A = globalArray;
            //leggiNumeriRandom(A);
            timer_start = MPI_Wtime();
        }

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Scatter(A, arraySize, MPI_INT, arrayLocale, arraySize, MPI_INT, MASTER, MPI_COMM_WORLD);

        quickSort(arrayLocale, arraySize);
        MPI_Barrier(MPI_COMM_WORLD);

        int log2Processori = (int)log2(num_processes);
        int idx = process_rank, passo = 0;
        int *arrayCorrente = arrayLocale;
        while (passo < log2Processori)
        {
            int dimArrayUnito = arraySize * (int)pow(2, passo + 1);
            if (idx % 2 == 0)
            {
                int *arrayUnito = new int[dimArrayUnito];
                int *arrayRicevuto = new int[dimArrayUnito / 2];

                int rankMittente = process_rank + (int)pow(2, passo);
                MPI_Recv(arrayRicevuto, dimArrayUnito / 2, MPI_INT, rankMittente, MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                unisci(arrayCorrente, arrayRicevuto, arrayUnito, dimArrayUnito);
                arrayCorrente = arrayUnito;
                delete[] arrayRicevuto;
            }
            else
            {
                int rankDestinazione = process_rank - (int)pow(2, passo);
                MPI_Send(arrayCorrente, dimArrayUnito / 2, MPI_INT, rankDestinazione, MASTER, MPI_COMM_WORLD);
                break;
            }
            passo++;
            idx /= 2;
        }

        MPI_Barrier(MPI_COMM_WORLD);
        if (process_rank == MASTER)
        {
            timer_end = MPI_Wtime();
            if (DEBUG)
                stampaArrayOrdinato(arrayCorrente);
            printInfo(2, arrayCorrente);
            printTime();
        }
        MPI_Barrier(MPI_COMM_WORLD);
        delete[] arrayCorrente;
    }
};
#endif