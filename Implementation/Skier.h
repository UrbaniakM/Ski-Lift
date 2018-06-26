#pragma once

#include <cstdio>
#include <ctime>
#include <list>
#include "Request.h"
#include "MPIManager.h"

class Skier{
	public:
		Skier(int rank, int size, int tokens, int weight);
		~Skier();
		void Loop();
		inline bool isLifted(){
			return std::clock() < timeout;
		}
	private:
		int tokens;
		int priority;
		int weight;
		int rank;
		int size;
		std::clock_t timeout;

		bool onLift;
		bool sentRequest;

		Request* requests; // size equal to number of skiers
		MPIManager communicationManager;

		inline void LeavingLift(){
			tokens += weight;
			onLift = false;
		}

		inline void EnterLift(){
			printf("node:\tenters lift, tokens before entering:\t",rank,tokens);
			tokens -= weight;
			communicationManager.SendTokens(tokens);
			priority = 0;
			requests[rank].priority = 0;
			communicationManager.SendCancelRequest(rank);
			timeout = std::clock() + 5000; // upon entering, wait 5 seconds for leaving lift
			onLift = true;
			sentRequest = false;
		}

		void PrintNodes(){
			printf("%d|\ttokens: %d\tpriority: %d\n",rank,tokens,priority);
		}

		void PrintRequests(){
			for(int i = 0; i < size; i++){
				if(requests[i].correct && i != rank){
					printf("%d\t%d:\t%d %d\n",rank,i,requests[i].weight,requests[i].priority);
				}
			}
		}

		void ReceiveRequest();

		void ReceivePriorityIncrement();

		void ReceiveTokens();

		void ReceiveCancelRequest();

		int IndexOfHighestPriorityRequest();
};
