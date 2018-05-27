#include "Skier.h"
#include "Request.h"
#include <stdio.h>
#include <iostream>


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

void Skier::loop()
{
	while (true) {

		consumeTokens();
		acceptSentRequests();
		acceptSentReleases();
		acceptSentTokens();
		std::cout<<"One revolution completed"<<std::endl;
		/*
		ImReadyController();
		SendRequestController();
		ReceivedRequestController();
		RecievedReleaseController();
		ReceivedTokensController();
		ImDoneController();
		*/
	}
}

void Skier::startWorking()
{
	myTokens -= this->weight;
	//Other things
	//Start timer
}

bool Skier::isWorking()
{
	return false;
}

void Skier::consumeTokens()
{
	if (allRequests.empty()) {
		return;
	}
	Request current = allRequests.best();
	Request me = Request::make(0, this->weight, this->rank);

	while (current.weight <= myTokens && !allRequests.empty()) {
		allRequests.erase(current);

		if (current == me) {
			SendPriorityIncrement(this->rank);
			startWorking();
				//Decreases myTokens
		}
		else {
			myTokens-= current.weight;
			SendTokens(current.weight);
		}

		if (sentRequests.contains(current)) {
			SendRelease(current);
			sentRequests.erase(current);
		}

		if (newRequests.contains(current)) {
			newRequests.erase(current);
		}

		current = allRequests.best();
	}
}

void Skier::acceptSentRequests()
{
	Request request;
	while (!newRequests.empty()) {
		request = newRequests.simple_pop();
		SendRequest(request);
	}

	request = ReceiveRequest();
	while (request.correct) {
		allRequests.insert(request);
		newRequests.insert(request);
	}
}

void Skier::acceptSentReleases()
{
	Request release = ReceiveRelease();
	while (release.correct) {
		if (sentRequests.contains(release)) {
			sentRequests.erase(release);
			allRequests.erase(release);
		}
		else {
			myTokens += release.weight;
		}
	}
}

void Skier::acceptSentTokens()
{
	int newTokens = ReceiveTokens();
	while (newTokens != -1)
	{
		myTokens += newTokens;
		newTokens = ReceiveTokens();
	}
}

void Skier::acceptPriorityIncrement()
{
	int pi_id = ReceivePriorityIncrement();
	while (pi_id != -1) {
		if (pi_id != this->rank) {
			SendPriorityIncrement(pi_id);
			this->allRequests.increasePriorityExcept(pi_id);
			this->newRequests.increasePriorityExcept(pi_id);
			this->sentRequests.increasePriorityExcept(pi_id);
		}
		pi_id = ReceivePriorityIncrement();
	}
}

void Skier::SendRequest(Request request)
{
	//MPI_Send( PRIORITY );
	//MPI_Send( WEIGHT );
	// TODO
}
Request Skier::ReceiveRequest()
{
	Request r;
	// TODO
	//if(!succeded)
	//	{r.correct=false;}
	r.correct = false;
	return r;
}
void Skier::SendTokens(int tokens)
{
}
int Skier::ReceiveTokens()
{
    //If fail return -1
    return -1;
}
void Skier::SendRelease(Request request)
{
}
Request Skier::ReceiveRelease()
{
	Request r;
	// TODO
	//if(!succeded)
	//	{r.correct=false;}
	r.correct = false;
	return r;
}
void Skier::SendPriorityIncrement(int id)
{
}
int Skier::ReceivePriorityIncrement()
{
	//returns -1 if no Received
	return -1;
}
int Skier::GetRank()
{
	return rank;
}
void Skier::PrintNodes()
{
	printf("left: %d; node: %d; right: %d\n", leftNode, rank, rightNode);
}


/*
void Skier::ImReadyController()
{
}

void Skier::ImDoneController()
{
}

void Skier::ReceivedRequestController()
{
}

void Skier::SendRequestController()
{
}

void Skier::ReceivedTokensController()
{
}

void Skier::RecievedReleaseController()
{
}
*/

bool Request::operator<(Request request)
{
	return (this->priority < request.priority);
}

bool Request::operator==(Request request)
{
	return (this->priority == request.priority) && (this->id == request.id);
}

Request::Request() {
	priority = 0;
	weight = 0;
}

Request::Request(int p, int w, int i) {
	priority = p;
	weight = w;
	id = i;
}

Request Request::make() {
	Request r;
	return r;
}

Request Request::make(int p, int w, int i) {
	Request r(p,w, i);
	return r;
}
