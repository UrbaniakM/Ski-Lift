#include "Skier.h"
#include "Request.h"
#include "PipeCtrl.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>

//#include "Constants.h"

Skier::Skier(int rank, int size, int tokens, int w)
{
	timeout = std::clock();
	isWorkingVar = false;
	this->rank = rank;
	priority = 0;
	myTokens = tokens;
	weight = w;
    //weight = rand()%(MAX_WEIGHT - MIN_WEIGHT) + MIN_WEIGHT;

	// create ring network
	if (rank == size-1) {
		leftNode = rank - 1;
		rightNode = 0;
	}
	else if (rank == 0) {
		leftNode = size-1;
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
	pipeCtrl = forkpipe(this->leftNode, this->rightNode);
	std::cout << "Rank: " << this->rank << std::endl;
	allRequests.insert(Request::make(priority, weight, rank));
	newRequests.insert(Request::make(priority, weight, rank));
	while (true) {

		consumeTokens();
		acceptSentRequests();
		acceptSentReleases();
		acceptSentTokens();
		if (!isWorking() && isWorkingVar) {
			isWorkingVar = false;
			myTokens += this->weight;
			std::cout << "Process ranked "<< rank <<"released "<< weight <<" tokens\n";
			allRequests.insert(Request::make(priority, weight, rank));
			newRequests.insert(Request::make(priority, weight, rank));
		}
		//std::cout<<"One revolution completed"<<std::endl;
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
	std::cout << "Process ranked " << rank << " has taken " << weight << " tokens\n";
	timeout = std::clock() +CLOCKS_PER_SEC * float(std::rand()%5) /10000;
	isWorkingVar = true;
}

bool Skier::isWorking()
{
	//std::cout << timeout - std::clock()<<std::endl;
	return std::clock() < timeout;
}

void Skier::consumeTokens()
{
	//std::cout << "consumeTokens" << std::endl;
	if (allRequests.empty()) {
		return;
	}
	Request current = allRequests.best();
	Request me = Request::make(0, this->weight, this->rank);

	std::cout << "make" << std::endl;
	while (current.weight <= myTokens && !allRequests.empty()) {
		allRequests.erase(current);

		std::cout << "erase" << std::endl;
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
		if(!allRequests.empty())
			current = allRequests.best();
	}
	if (myTokens > 0 && !allRequests.empty() && !(current == me)) {
		SendTokens(myTokens);
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
		int rest = sentRequests.decrease(release);
		release.weight -= rest;
		allRequests.decrease(release);
		myTokens += rest;
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
	pipeCtrl.sendRequest(request.priority, request.weight, request.id, rightNode);
}

Request Skier::ReceiveRequest()
{
	return pipeCtrl.readRequest();
}
void Skier::SendTokens(int tokens){
	pipeCtrl.sendTokens(tokens, leftNode);
}

int Skier::ReceiveTokens()
{
	return pipeCtrl.readTokens();
}
void Skier::SendRelease(Request request){
	pipeCtrl.sendRelease(request.priority, request.weight, request.id, rightNode);
}

Request Skier::ReceiveRelease()
{
	return pipeCtrl.readRelease();
}
void Skier::SendPriorityIncrement(int id)
{
	pipeCtrl.sendPriorityIncrement(id, rightNode);
}
int Skier::ReceivePriorityIncrement()
{
	return pipeCtrl.readPriorityIncrement();
}
int Skier::GetRank()
{
	return rank;
}
void Skier::PrintNodes()
{
	printf("left: %d; node: %d; right: %d\n", leftNode, rank, rightNode);
	printf("tokens: %d; weight: %d; \n", myTokens, weight);
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
