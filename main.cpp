#include "bitonic.h"
#include "quicksort.h"
#include "oddEven.h"

int controllaSize(int oldSize)
{
    /**
    *Controlla che la stringa possa essere distribuita 
    * equamente su tutti i processori
    * Esempio:
    * processori (8), size (125)
    * 125 % 8 --> 5 (ovvero, 5 numeri rimangono non distribuiti
    * sui processori per essere ordinati
    * QUINDI, 8 - 5 --> 3 (dobbiamo incrementare la size di 3, per poter 
    * avere numeri suffcienti per poterli distribuire sui vari processori
    */
    /*
    if (oldSize % num_processes != 0)
        oldSize += num_processes - (oldSize % num_processes);
    return oldSize;*/

    if (oldSize < num_processes)
        return num_processes;

    auto res = (double)log2(oldSize);

    if (res != (int)res)
        oldSize = pow(2, (int)res + 1);
    return oldSize;
}

int controllaProcessori(int nProcessori)
{
    auto res = (double)log2(nProcessori);

    if (res != (int)res)
        nProcessori = pow(2, (int)res + 1);

    return nProcessori;
}
int main(int argc, char *argv[])
{

    bitonic b;
    quicksort q;
    oddEven o;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    size = controllaSize(atoi(argv[1]));

    if (argv[2] != NULL)
        showGraphic = (bool)atoi(argv[2]);

    if (size < LIMITE)
        numeriDaStampare = size;

    if (process_rank == MASTER)
    {
        if (num_processes != controllaProcessori(num_processes))
        {
            printf("Errore! Si prega di utilizzare un numero di processori che sia potenza di 2...\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        globalArray = new int[size];
        printf("Inizio generazione\n");
        srand(time(NULL) + MPI_Wtime() * num_processes);
        double start = MPI_Wtime();

        for (int i = 0; i < size; i++)
            globalArray[i] = (rand() % (size - 1)) + 1;
        printf("Fine generazione %f\n", MPI_Wtime() - start);
        start = MPI_Wtime();
        scriviSuFile(globalArray);
        printf("Fine scrittura su file in %f\n", MPI_Wtime() - start);
        printf("---------------------------------------------------\n");

        if (showGraphic)
        {
            al_init();
            al_init_primitives_addon();
            display = al_create_display(WINDOWSIZE, WINDOWSIZE);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    arraySize = size / num_processes;

    b.start();

    MPI_Barrier(MPI_COMM_WORLD);

    q.start();

    MPI_Barrier(MPI_COMM_WORLD);

    o.start();

    if (process_rank == MASTER && display != NULL)
        al_destroy_display(display);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    delete[] globalArray;
    return 0;
}
