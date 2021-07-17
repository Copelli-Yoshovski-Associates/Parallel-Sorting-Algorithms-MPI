#ifndef BITONIC_H
#define BITONIC_H
#include "settings.h"

class bitonic
{
private:
    enum
    {
        HIGH = 0,
        LOW
    };

    int *listaLocale, *listaTemp, *sequenzaUnita, *Print;

    void bitonicSort(int *arr, int n)
    {
        int k, j, l, i, temp;
        for (k = 2; k <= n; k *= 2)
            for (j = k / 2; j > 0; j /= 2)
            {
                for (i = 0; i < n; i++)
                {
                    l = i ^ j;
                    if (l > i)
                        if (((i & k) == 0) && (arr[i] > arr[l]) || (((i & k) != 0) && (arr[i] < arr[l])))
                        {
                            temp = arr[i];
                            arr[i] = arr[l];
                            arr[l] = temp;
                        }
                }
                if (showGraphic)
                {
                    showGraphics(arr);
                    MPI_Barrier(MPI_COMM_WORLD);
                }
            }
    }

    void unisciSequenza(int arraySize, int *listaLocale, int direzione, int partner)
    {
        MPI_Status status;
        listaTemp = new int[arraySize];
        MPI_Sendrecv(listaLocale, arraySize, MPI_INT, partner, 0, listaTemp, arraySize, MPI_INT, partner, 0, MPI_COMM_WORLD, &status);
        if (direzione == HIGH)
            unisciSequenzaHigh(arraySize, listaLocale, listaTemp);
        else
            unisciSequenzaLow(arraySize, listaLocale, listaTemp);

        delete[] listaTemp;
    }

    void unisciSequenze(int arraySize, int *lista1, int *lista2, bool low)
    {
        int i, idx1 = 0, idx2 = 0;
        sequenzaUnita = new int[arraySize];
        if (low)
        {
            for (i = 0; i < arraySize; i++)
                if (lista1[idx1] <= lista2[idx2])
                {
                    sequenzaUnita[i] = lista1[idx1];
                    idx1++;
                }
                else
                {
                    sequenzaUnita[i] = lista2[idx2];
                    idx2++;
                }
        }
        else
        {
            idx1 = arraySize - 1;
            idx2 = arraySize - 1;
            for (i = arraySize - 1; i >= 0; i--)
                if (lista1[idx1] >= lista2[idx2])
                {
                    sequenzaUnita[i] = lista1[idx1];
                    idx1--;
                }
                else
                {
                    sequenzaUnita[i] = lista2[idx2];
                    idx2--;
                }
        }

        for (i = 0; i < arraySize; i++)
            lista1[i] = sequenzaUnita[i];

        delete[] sequenzaUnita;
    }

    //Wrapper per sequenze Low
    void unisciSequenzaLow(int arraySize, int *lista1, int *lista2) { unisciSequenze(arraySize, lista1, lista2, true); }
    //Wrapper per sequenze High
    void unisciSequenzaHigh(int arraySize, int *lista1, int *lista2) { unisciSequenze(arraySize, lista1, lista2, false); }

    //Wrapper per bitonic Sort Crescente
    void bitonicSortCrescente(int arraySize, int *listaLocale, int dimProcessori) { bitonicSortDirezione(arraySize, listaLocale, dimProcessori, true); }
    //Wrapper per bitonic Sort Decrescente
    void bitonicSortDecrescente(int arraySize, int *listaLocale, int dimProcessori) { bitonicSortDirezione(arraySize, listaLocale, dimProcessori, false); }

    void bitonicSortDirezione(int arraySize, int *listaLocale, int dimProcessori, bool cresc)
    {
        int partner;
        int dimensioneProcessore = log2(dimProcessori);

        // shift sinistro di (dimensioneProcessori-1) posizioni
        unsigned bitwise = 1 << (dimensioneProcessore - 1);

        for (int i = 0; i < dimensioneProcessore; i++)
        {
            // XOR Esclusivo (restituisce TRUE se e solo se gli ingressi sono diversi tra di loro)
            partner = process_rank ^ bitwise;
            if ((process_rank < partner && cresc) || (process_rank > partner && !cresc))
                unisciSequenza(arraySize, listaLocale, LOW, partner);
            else
                unisciSequenza(arraySize, listaLocale, HIGH, partner);
            //shift destro
            bitwise = bitwise >> 1;
        }
    }

public:
    bitonic() {}
    void start()
    {
        int *array;

        listaLocale = new int[arraySize];

        if (process_rank == MASTER)
        {
            array = globalArray;
            //leggiNumeriRandom(array);
            timer_start = MPI_Wtime();
        }
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Scatter(array, arraySize, MPI_INT, listaLocale, arraySize, MPI_INT, MASTER, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        if (DEBUG)
        {
            printf("Processore %d: ", process_rank);
            for (int i = 0; i < arraySize; i++)
                printf("%d ", listaLocale[i]);
            printf("\n");
            MPI_Barrier(MPI_COMM_WORLD);
        }

        bitonicSort(listaLocale, arraySize);

        unsigned andBit = 2;
        for (int dimProcessori = 2; dimProcessori <= num_processes; dimProcessori *= 2)
        {
            // bitwise & effettua l'AND logico bit a bit come nell'esempio 2 & 3 --> 010 && 011 = 010
            if ((process_rank & andBit) == 0)
                bitonicSortCrescente(arraySize, listaLocale, dimProcessori);
            else
                bitonicSortDecrescente(arraySize, listaLocale, dimProcessori);
            MPI_Barrier(MPI_COMM_WORLD);

            //effettua uno shift sinistro di una posizione
            andBit = andBit << 1;
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if (process_rank == MASTER)
        {
            timer_end = MPI_Wtime();
            Print = new int[size];
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Gather(listaLocale, arraySize, MPI_INT, Print, arraySize, MPI_INT, MASTER, MPI_COMM_WORLD);

        if (process_rank == MASTER)
        {
            if (DEBUG)
                stampaArrayOrdinato(Print);

            printInfo(1, Print);
            printTime();
            delete[] Print;
            delete[] listaLocale;
        }
    }
};
#endif
