#pragma once
#include <list>
class Skier
{
public:
	struct Request {
		int priority;
		int weight;
	};

	Skier(int rank, int size);
	~Skier();
	void SendRequest();
	void ReceiveRequest();
	void SendTokens();
	void ReceiveTokens();
	void SendRelease();
	void ReceiveRelease();
	int GetRank();
	void PrintNodes();
	void loop();
private:
	std::list<Request> requests;
	//Skier* leftNode;
	//Skier* rightNode;
	int leftNode;
	int rightNode;
	int priority;
	int weight;
	int rank;
	void ImReadyController();
	void ImDoneController();
	void ReceivedRequestController();
	void SendRequestController();
	void ReceivedTokensController();
	void RecievedReleaseController();
};
