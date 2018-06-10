#pragma once
#include <set>
#include <iostream>
#include <cstdio>
#include <ctime>
#include "MySortedList.h"
#include "ThreadsCtrl.h"
#include "Request.h"

class Skier
{
public:

	Skier(int rank, int size, int tokens, int weight);
	~Skier();

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
	std::clock_t timeout;
	ThreadsCtrl pipeCtrl;


	void consumeTokens();
	void acceptSentRequests();
	void acceptSentReleases();
	void acceptSentTokens();
	void acceptPriorityIncrement();
};
