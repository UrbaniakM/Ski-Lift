#include "mpi.h"
#include <stdio.h>
#include "Skier.cpp"

int main(int argc, char **argv){
    int rank, size;
    // argv[1] - number of skiers
    // argv[2] - capacity of lift
    //printf("%s\n",argv[1]);

    MPI_Init(&argc, &argv);

    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    Skier skier = Skier(rank, size);
    skier.PrintNodes();

    MPI_Finalize();
}
