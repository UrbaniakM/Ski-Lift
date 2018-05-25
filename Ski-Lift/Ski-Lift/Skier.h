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
};
