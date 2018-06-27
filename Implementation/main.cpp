#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Skier.h"
#include "mpi.h"

#define LIFT_CAPACITY 103
#define SKIER_WEIGHT 45

int main(int argc, char **argv) {
	int rank, size;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

	int tokens = LIFT_CAPACITY / size;
    if(rank == 0){
		tokens += LIFT_CAPACITY % size;
    }
    Skier skier = Skier(rank, size, tokens, SKIER_WEIGHT);
    skier.Loop();

    printf("Skier\t%d closed\n",rank);

    MPI_Finalize();
}
