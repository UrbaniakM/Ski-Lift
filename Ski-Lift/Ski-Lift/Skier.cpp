#include "Skier.h"
#include "Request.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>

// MESSAGE TAGS:
#define REQUEST_TAG 0
#define RELEASE_TAG 1
#define PRIORITY_TAG 2
#define TOKENS_TAG 3

Skier::Skier(int rank, int size, int tokens, int w)
{
	timeout = std::clock();
	isWorkingVar = false;
	this->rank = rank;
	priority = 0;
	myTokens = tokens;
	weight = w;
	// Create ring network
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

void Skier::loop()
{
	std::cout << this->rank << ": ____" << std::endl;
	//MPI_Barrier(MPI_COMM_WORLD);
	std::cout << this->rank << ": MPI_Barrier" << std::endl;
	sleep(1);
	std::cout << this->rank << ": sleep" << std::endl;
	std::cout << this->rank << ": startRightMutex, startLeftMutex" << std::endl;
	std::cout << "Rank: " << this->rank << std::endl;
	allRequests.insert(Request::make(priority, weight, rank));
	std::cout << this->rank << ": allRequests" << std::endl;
	newRequests.insert(Request::make(priority, weight, rank));
	std::cout << this->rank << ": newRequests" << std::endl;
	std::clock_t deltaClock = std::clock();
	while (true) {
		//std::cout << this->rank << ": ____"<<std::endl;
		consumeTokens();
		//std::cout << this->rank << ": consumeTokens" << std::endl;
		acceptSentRequests();
		//std::cout << this->rank << ": acceptSentRequests" << std::endl;
		acceptSentReleases();
		//std::cout << this->rank << ": acceptSentReleases" << std::endl;
		acceptSentTokens();
		//std::cout << this->rank << ": acceptSentTokens" << std::endl;
		if (!isWorking() && isWorkingVar) {
			isWorkingVar = false;
			myTokens += this->weight;
			std::cout << "Process ranked "<< rank <<" released "<< weight <<" tokens, and has "
				<<myTokens<<" tokens\n";
			allRequests.insert(Request::make(priority, weight, rank));
			newRequests.insert(Request::make(priority, weight, rank));
			std::cout << "loop()::allRequests.size() :" << allRequests.size() << std::endl;
			std::cout << "loop()::newRequests.size() :" << newRequests.size() << std::endl;
		}
		if (std::clock() - deltaClock > CLOCKS_PER_SEC / 3.0) {
			deltaClock = std::clock();
		}
		//std::cout << this->rank << ": isWorking" << std::endl;
		sleep(0.1);
	}
}

void Skier::startWorking()
{
	myTokens -= this->weight;
	std::cout << "Process ranked " << rank << " has taken " << weight << " tokens, and has "
		<< myTokens << " tokens\n";
	timeout = std::clock() + CLOCKS_PER_SEC * float(1 + std::rand() % 5);
	isWorkingVar = true;
}

bool Skier::isWorking()
{
	return std::clock() < timeout;
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

		std::cout << "current.weight=" << current.weight << std::endl;
		std::cout << "myTokens=" << myTokens << std::endl;
		if (current == me) {
			SendPriorityIncrement(this->rank);
			startWorking();
				// Decreases myTokens (number of hold tokens)
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
		sentRequests.insert(request);
		std::cout << "acceptSentRequests::sentRequests.size() :" << sentRequests.size() << std::endl;
	}

	request = ReceiveRequest();
	while (request.correct) {
		allRequests.insert(request);
		std::cout << "acceptSentRequests::allRequests.size() :" << allRequests.size() << std::endl;
		newRequests.insert(request);
		std::cout << "acceptSentRequests::newRequests.size() :" << newRequests.size() << std::endl;
		request = ReceiveRequest();
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


///
/// Sends Request (3 ints - priority, weight and id) to the Right Node.
/// MPI Communication
/// Type: Send
/// Message tag: REQUEST_TAG
/// Node: rightNode
/// Buffer size: 3
/// Buffer datatype: MPI_INT
///
void Skier::SendRequest(Request request)
{
	std::cout << "SendRequest" << std::endl;
    int arr[3] = { request.priority, request.weight, request.id};
	MPI_Send(arr, 3, MPI_INT, rightNode, REQUEST_TAG, MPI_COMM_WORLD);
	//std::cout << "S" << std::endl;
	//delete arr;
}

///
/// Receives Request (3 ints - priority, weight and id) from Left Node if encounters in buffer, otherwise request is marked as incorrect. 
/// MPI Communication
/// Type: IRecv
/// Message tag: REQUEST_TAG
/// Node: leftNode
/// Buffer size: 3
/// Buffer datatype: MPI_INT
///
Request Skier::ReceiveRequest()
{
    Request r;
    if(triedReceiveLeftRequest){
        int flag;
        MPI_Request_get_status(leftReceiveRequest,&flag, MPI_STATUS_IGNORE);
        if(flag){
            r.priority = leftBufferRequest[0];
            r.weight = leftBufferRequest[1];
            r.id = leftBufferRequest[2];
            r.correct = true;
            triedReceiveLeftRequest = false;
			//std::cout << "R" << std::endl;
			//std::cout << "R" << std::endl;
        } else {
            r.correct = false;
        }
    }
    else {
        MPI_Irecv(leftBufferRequest, 3, MPI_INT, leftNode, REQUEST_TAG, MPI_COMM_WORLD, &leftReceiveRequest);
        triedReceiveLeftRequest = true;
        r.correct = false;
    }
	return r;
}

///
/// Sends Tokens (1 int) to the Left Node.
/// MPI Communication
/// Type: Send
/// Message tag: TOKENS_TAG
/// Node: leftNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
void Skier::SendTokens(int tokens){
    int arr[1] = { tokens };
    MPI_Send(arr, 1, MPI_INT, leftNode, TOKENS_TAG, MPI_COMM_WORLD);
	//std::cout << "S" << std::endl;
	//delete arr;
}


///
/// Receives Tokens (1 int) from Right Node if encounters in buffer, otherwise returns -1. 
/// MPI Communication
/// Type: IRecv
/// Message tag: TOKENS_TAG
/// Node: rightNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
int Skier::ReceiveTokens()
{
    int tokens = -1;
    if(triedReceiveRightTokens){
        int flag;
        MPI_Request_get_status(rightReceiveTokens,&flag, MPI_STATUS_IGNORE);
        if(flag){
            tokens = rightBufferTokens[0];
            triedReceiveRightTokens = false;
			//std::cout << "R" << std::endl;
        }
    }
    else {
        MPI_Irecv(rightBufferTokens, 1, MPI_INT, rightNode, TOKENS_TAG, MPI_COMM_WORLD, &rightReceiveTokens);
        triedReceiveRightTokens = true;
        
    }
    return tokens;
}

///
/// Sends Release-Request (3 ints - priority, weight and id) to the Right Node.
/// MPI Communication
/// Type: Send
/// Message tag: RELEASE_TAG
/// Node: rightNode
/// Buffer size: 3
/// Buffer datatype: MPI_INT
///
void Skier::SendRelease(Request request){
    int arr[3] = { request.priority, request.weight, request.id};
	MPI_Send(arr, 3, MPI_INT, rightNode, RELEASE_TAG, MPI_COMM_WORLD);
	//std::cout << "S" << std::endl;
	//delete arr;
}

///
/// Receives Release-Request (3 ints - priority, weight and id) from Left Node if encounters in buffer, otherwise release-request is marked as incorrect. 
/// MPI Communication
/// Type: IRecv
/// Message tag: RELEASE_TAG
/// Node: leftNode
/// Buffer size: 3
/// Buffer datatype: MPI_INT
///
Request Skier::ReceiveRelease()
{
    Request r;
    r.correct = false;
    if(triedReceiveLeftRelease){
        int flag;
        MPI_Request_get_status(leftReceiveRelease,&flag, MPI_STATUS_IGNORE);
        if(flag){
            r.priority = leftBufferRelease[0];
            r.weight = leftBufferRelease[1];
            r.id = leftBufferRelease[2];
            r.correct = true;
            triedReceiveLeftRelease = false;
			////std::cout << "R" << std::endl;
        }
    }
    else {
        MPI_Irecv(leftBufferRelease, 3, MPI_INT, leftNode, RELEASE_TAG, MPI_COMM_WORLD, &leftReceiveRelease);
        triedReceiveLeftRelease = true;
    }
	return r;
}

///
/// Send Priority Increment by Id (1 int) to the Right Node.
/// MPI Communication
/// Type: Send
/// Message tag: PRIORITY_TAG
/// Node: leftNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
void Skier::SendPriorityIncrement(int id)
{
    int arr[1] = { id };
	MPI_Send(arr, 1, MPI_INT, rightNode, PRIORITY_TAG, MPI_COMM_WORLD);
	//std::cout << "S" << std::endl;
	//delete arr;
}

///
/// Receives Priority Increment by Id (1 int) from Left Node if encounters in buffer, otherwise returns -1. 
/// MPI Communication
/// Type: IRecv
/// Message tag: PRIORITY_TAG
/// Node: rightNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
int Skier::ReceivePriorityIncrement()
{
    int id = -1;
    if(triedReceiveLeftPriority){
        int flag;
        MPI_Request_get_status(leftReceivePriority,&flag, MPI_STATUS_IGNORE);
        if(flag){
            id = leftBufferPriority[0];
            triedReceiveLeftPriority = false;
			//std::cout << "R" << std::endl;
        }
    }
    else {
        MPI_Irecv(leftBufferPriority, 1, MPI_INT, leftNode, PRIORITY_TAG, MPI_COMM_WORLD, &leftReceivePriority);
        triedReceiveLeftPriority = true;
    }
    return id;
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
