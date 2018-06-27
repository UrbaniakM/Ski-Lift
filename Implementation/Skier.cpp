#include "Skier.h"
#include "Request.h"
#include <stdio.h>
#include <unistd.h>

Skier::Skier(int rank, int size, int tokens, int weight)
{
	priority = rank;
	timeout = std::clock();
	onLift = false;
	sentRequest = false;

	this->size = size;
	this->tokens = tokens;
	this->weight = weight;
	this->rank = rank;

	// Create ring network
	int leftNode;
	int rightNode;
	if (rank == size - 1)
	{
		leftNode = rank - 1;
		rightNode = 0;
	}
	else if (rank == 0)
	{
		leftNode = size - 1;
		rightNode = rank + 1;
	}
	else
	{
		leftNode = rank - 1;
		rightNode = rank + 1;
	}

	this->communicationManager.RightNode(rightNode);
	this->communicationManager.LeftNode(leftNode);

	this->requests = new Request[size];
	for (int i = 0; i < size; i++)
	{
		requests[i].priority = 0;
		requests[i].weight = 0;
		requests[i].correct = true;
	}
}

Skier::~Skier()
{
	delete (requests);
}

void Skier::ReceiveRequest()
{
	Request request = communicationManager.ReceiveRequest();
	if ((request.correct) && (request.id != rank))
	{
		requests[request.id].correct = true;
		requests[request.id].id = request.id;
		requests[request.id].priority = request.priority;
		requests[request.id].weight = request.weight;
		communicationManager.SendRequest(request);
	}
}

void Skier::ReceivePriorityIncrement()
{
	int id = communicationManager.ReceivePriorityIncrement();
	if (id >= 0 && id != rank)
	{
		requests[id].priority++;
		communicationManager.SendPriorityIncrement(id);
	}
}

void Skier::ReceiveTokens()
{
	TokensStruct tokensStruct = communicationManager.ReceiveTokens();
	if (tokensStruct.correct)
	{
		if (tokensStruct.target == rank || tokensStruct.target == -1)
			tokens += tokensStruct.tokens;
		else
		{
			requests[tokensStruct.target].weight -= tokensStruct.tokens;
			if (requests[tokensStruct.target].weight <= 0)
			{
				//requests[tokensStruct.target].correct = false;
				requests[tokensStruct.target].weight = 0;
				//communicationManager.SendCancelRequest(tokensStruct.target);
			}
			communicationManager.SendTokens(tokensStruct);
		}
	}
}

void Skier::ReceiveCancelRequest()
{
	int receiveVal = communicationManager.ReceiveCancelRequest();
	//if (receiveVal >= 0 && requests[receiveVal].correct && receiveVal != rank)
	if (receiveVal >= 0 && requests[receiveVal].weight > 0 && receiveVal != rank)
	{
		//requests[receiveVal].correct = false;
		requests[receiveVal].weight = 0;
		communicationManager.SendCancelRequest(receiveVal);
	}
}

int Skier::IndexOfHighestPriorityRequest()
{
	int index = -1;
	int highestPriority = -1;
	for (int i = 0; i < size; i++)
	{
		//if (requests[i].correct && requests[i].priority >= highestPriority)
		if (requests[i].weight > 0 && requests[i].priority >= highestPriority)
		{
			index = i;
			highestPriority = requests[i].priority;
		}
	}
	return index;
}

int Skier::IndexProcessStarving()
{
	for (int i = 0; i < size; i++)
	{
		if (requests[i].priority > priority * 10)
		{
			return i;
		}
	}
	return -1;
}

void Skier::Loop()
{
	while (true)
	{
		ReceiveCancelRequest();
		ReceiveRequest();
		ReceivePriorityIncrement();
		ReceiveTokens();
		if (!onLift && !sentRequest && canEnter())
		{
			sentRequest = true;
			Request request;
			request.priority = priority;
			request.weight = weight;
			request.correct = true;
			request.id = rank;
			requests[rank] = request;
			//printf("SENDING REQUEST,%d\n",rank);
			communicationManager.SendRequest(request);
			//printf("FINISHED SENDING REQUEST,%d\n",rank);
		}

		if (onLift && !isLifted())
		{
			LeaveLift();
			//PrintRequests();
		}
		if (rank == 0)
		{
			//PrintNodes();
			//	PrintRequests();
			//sleep(2);
		}

		if (tokens > 0)
		{
			int indexProcessStarving = IndexProcessStarving();
			if (indexProcessStarving >= 0)
			{
				TokensStruct tokensStruct;
				tokensStruct.target = indexProcessStarving;
				tokensStruct.tokens = tokens;
				communicationManager.SendTokens(tokensStruct);
				tokens = 0;
				requests[indexProcessStarving].priority = 0;
			}
			else
			{
				int indexHighestPriority = IndexOfHighestPriorityRequest();
				if (indexHighestPriority >= 0)
				{
					if (indexHighestPriority == rank && !onLift)
					{
						if (tokens >= weight && canEnter())
							EnterLift();
					}
					else if (indexHighestPriority != rank)
					{
						int possibleTokens = tokens >= requests[indexHighestPriority].weight ? requests[indexHighestPriority].weight : tokens;
						TokensStruct tokensStruct;
						tokensStruct.target = indexHighestPriority;
						tokensStruct.tokens = possibleTokens;
						communicationManager.SendTokens(tokensStruct);
						priority++;
						requests[rank].priority++;
						communicationManager.SendPriorityIncrement(rank);
						requests[indexHighestPriority].weight -= possibleTokens;
						tokens -= possibleTokens;
						if (requests[indexHighestPriority].weight == 0)
						{
							//requests[indexHighestPriority].correct = false;
							//communicationManager.SendCancelRequest(indexHighestPriority);
						}
						else
						{
							communicationManager.SendRequest(requests[indexHighestPriority]);
						}
					}
				}
			}
		}
	}
}