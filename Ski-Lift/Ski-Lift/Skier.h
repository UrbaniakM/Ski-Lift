#pragma once
#include <set>
#include <iostream>
#include <cstdio>
#include <ctime>
#include "MySortedList.h"
//#include "ThreadsCtrl.h"
#include "Request.h"
#include "main.h"
#include "mpi.h"

class Skier
{
public:

	Skier(int rank, int size, int tokens, int weight);
	~Skier() { }

	void SendRequest(Request request);
	Request ReceiveRequest();
	void SendTokens(int tokens);
	int ReceiveTokens();
	void SendRelease(Request request);
	Request ReceiveRelease();

	void SendPriorityIncrement(int id);
	int ReceivePriorityIncrement();

	int GetRank();
	void PrintNodes();
	void loop();
	void startWorking();
	bool isWorking();
private:
	MySortedList allRequests;
	MySortedList newRequests;
	MySortedList sentRequests;
	bool isWorkingVar;
	int myTokens;
	int leftNode;
	int rightNode;
	int priority;
	int weight;
	int rank;
    
    bool triedReceiveLeftRequest = false;
    bool triedReceiveLeftRelease = false;
    bool triedReceiveLeftPriority = false;
    bool triedReceiveRightTokens = false;
    MPI_Request leftReceiveRequest;
    MPI_Request leftReceiveRelease;
    MPI_Request leftReceivePriority;
    MPI_Request rightReceiveTokens;
    int leftBufferRequest[3];
    int leftBufferRelease[3];
    int leftBufferPriority[1];
    int rightBufferTokens[1];

	std::clock_t timeout;

	void consumeTokens();
	void acceptSentRequests();
	void acceptSentReleases();
	void acceptSentTokens();
	void acceptPriorityIncrement();
};
