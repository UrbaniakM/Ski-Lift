#include "stdafx.h"
#include "Skier.h"


Skier::Skier(int rank, int size) 
{
	this->rank = rank;
	priority = 0;
	// TODO: rand weight

	// create ring network
	if (rank == size) {
		leftNode = rank - 1;
		rightNode = 0;
	}
	else if (rank == 0) {
		leftNode = size;
		rightNode = rank + 1;
	}
	else {
		leftNode = rank - 1;
		rightNode = rank + 1;
	}
}


Skier::~Skier()
{
}

void Skier::SendRequest() 
{
	Request request = Request();
	request.priority = this->priority;
	request.weight = this->weight;
	//MPI_Send( PRIORITY );
	//MPI_Send( WEIGHT );
	// TODO
}
void Skier::ReceiveRequest() 
{
	// TODO
}
int Skier::GetRank() 
{
	return rank;
}
void Skier::PrintNodes() 
{
	printf("left: %d; node: %d; right: %d\n", leftNode, rank, rightNode);
}
void Skier::loop() 
{
	while (true) {
		// TODO
	}
}