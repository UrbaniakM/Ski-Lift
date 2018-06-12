#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Skier.h"
#include "main.h"
#include "mpi.h"

int main(int argc, char **argv) {
	int rank, size;
	srand(time(NULL));

    MPI_Init(&argc, &argv);


    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
	srand(std::rand() + rank);

	int tokens = LIFT_CAPACITY / size;
    if(rank == 0){
		tokens += LIFT_CAPACITY % size;
    }
	int weight = LIFT_CAPACITY / size + 1;
    Skier skier = Skier(rank, size, tokens, weight);
    skier.PrintNodes();
    skier.loop();

    MPI_Finalize();
}
