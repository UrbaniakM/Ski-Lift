#include "Skier.h"
#include "Request.h"
#include <stdio.h>
#include <unistd.h>

Skier::Skier(int rank, int size, int tokens, int weight){
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

	this->communicationManager.RightNode(rightNode);
	this->communicationManager.LeftNode(leftNode);

	this->requests = new Request[size];
}

Skier::~Skier(){
	delete(requests);
}

void Skier::ReceiveRequest(){
	Request request = communicationManager.ReceiveRequest();
	if( (request.correct) && (request.id != rank) ){
		requests[request.id] = request;
		communicationManager.SendRequest(request);
		/*if(tokens > 0){
			int possibleTokens = request.weight > tokens ? tokens : request.weight;
			tokens -= possibleTokens;
			requests[request.id].weight -= possibleTokens;
			communicationManager.SendTokens(possibleTokens);
			if(requests[request.id].weight == 0){
				requests[request.id].correct = false;
			} else {
				communicationManager.SendRequest(requests[request.id]);
			}
			priority++;
			requests[rank].priority = priority;
			communicationManager.SendPriorityIncrement(rank);
		} else {
			communicationManager.SendRequest(request);
		}*/
	}
}

void Skier::ReceivePriorityIncrement(){
	PriorityIncrement priorityIncrement = communicationManager.ReceivePriorityIncrement();
	if(priorityIncrement.correct){
		if(requests[priorityIncrement.id].correct && priorityIncrement.id != rank){
			requests[priorityIncrement.id].priority += priorityIncrement.newPriority;
			communicationManager.SendPriorityIncrement(priorityIncrement);
		}
	}
}

void Skier::ReceiveTokens(){
	tokens += communicationManager.ReceiveTokens();
}

void Skier::ReceiveCancelRequest(){
	int receiveVal = communicationManager.ReceiveCancelRequest();
	if(receiveVal >= 0 && requests[receiveVal].correct){
		requests[receiveVal].correct = false;
		communicationManager.SendCancelRequest(receiveVal);
	}
}

int Skier::IndexOfHighestPriorityRequest(){
	int index = -1;
	int highestPriority = -1;
	for(int i = 0; i < size; i++){
		//if(i != rank && requests[i].correct && requests[i].priority >= highestPriority){
		if(requests[i].correct && requests[i].priority >= highestPriority){
			index = i;
			highestPriority = requests[i].priority;
		}
	}
	return index;
}


void Skier::Loop(){
	int attempts = 0;
	while(true){
		ReceiveRequest();
		ReceivePriorityIncrement();
		ReceiveCancelRequest();
		ReceiveTokens();
		if(!onLift && !sentRequest){
			sentRequest = true;
			Request request;
			request.priority = priority;
			request.weight = weight;
			request.correct = true;
			request.id = rank;
			requests[rank] = request;
			communicationManager.SendRequest(request);
		}
		//PrintRequests();
		
		if(tokens > 0){
			if(!onLift && tokens >= weight){
				EnterLift();
			}
			else {
				if(attempts == 100){
					attempts = 0;
					communicationManager.SendTokens(tokens);
					priority += tokens;
					requests[rank].priority += tokens;
					PriorityIncrement priorityIncrement;
					priorityIncrement.id = rank;
					priorityIncrement.newPriority = tokens;
					printf("%d forced to send %d tokens\n",rank,tokens);
					tokens = 0;
					sleep(1);
					communicationManager.SendPriorityIncrement(priorityIncrement);
				}
				int indexHighestPriority = IndexOfHighestPriorityRequest();
				if(indexHighestPriority >= 0){
					if(indexHighestPriority == rank && !onLift){
						if(tokens >= weight){
							EnterLift();
						} else
							attempts++;
					}else if(indexHighestPriority != rank){
						//printf("%d send %d tokens",rank, tokens);
						communicationManager.SendTokens(tokens);
						priority += tokens;
						requests[rank].priority += tokens;
						PriorityIncrement priorityIncrement;
						priorityIncrement.id = rank;
						priorityIncrement.newPriority = tokens;
						communicationManager.SendPriorityIncrement(priorityIncrement);
						requests[indexHighestPriority].weight -= tokens;
						if(requests[indexHighestPriority].weight == 0){
							requests[indexHighestPriority].correct = false;
							communicationManager.SendCancelRequest(indexHighestPriority);
						} else {
							communicationManager.SendRequest(requests[indexHighestPriority]);
						}
						tokens = 0;
					}
				}
			}
		}
	}
}