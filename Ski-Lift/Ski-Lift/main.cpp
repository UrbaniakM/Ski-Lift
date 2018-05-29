#include <stdio.h>
#include <iostream>
#include "Skier.h"
#include "PipeCtrl.h"
#include "main.h"
#include "mpi.h"

int main(int argc, char **argv) {
	int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

	int tokens = LIFT_CAPACITY / size;
    if(rank == 0){
		tokens += LIFT_CAPACITY % size;
    }

    Skier skier = Skier(rank, size, tokens);
    skier.PrintNodes();
	getchar();
    skier.loop();

    MPI_Finalize();
}
