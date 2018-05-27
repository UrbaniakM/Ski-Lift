#pragma once
#include <set>
#include "MySortedList.h"
#include "Request.h"

class Skier
{
public:

	Skier(int rank, int size);
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
	//Skier* leftNode;
	//Skier* rightNode;
	int myTokens;
	int leftNode;
	int rightNode;
	int priority;
	int weight;
	int rank;


	void consumeTokens();
	void acceptSentRequests();
	void acceptSentReleases();
	void acceptSentTokens();
	void acceptPriorityIncrement();
	/*
	void ImReadyController();
	void ImDoneController();
	void ReceivedRequestController();
	void SendRequestController();
	void ReceivedTokensController();
	void RecievedReleaseController();
	*/

};
