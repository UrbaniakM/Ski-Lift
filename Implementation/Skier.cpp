#include "mpi.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <list>

#include "Constants.h"
#include "Request.h"
#include "MySortedList.h"

class Skier {
    private:
        std::list<Request> requests;
		MySortedList allRequests;
		MySortedList newRequests;
		MySortedList sentRequests;

        int leftNode;
        int rightNode;
        int priority;
        int weight;
        int rank;
        
        int numberOfTokens;

        void SendRequest(Request request){
            int message[2];
            message[0] = request.priority;
            message[1] = request.weight;
            MPI_Send( message, 2, MPI_INT, rightNode, 0, MPI_COMM_WORLD); // MPI_Isend > MPI_Send
        }

        void SendRequest(){
            Request request = Request();
            request.priority = this->priority;
            request.weight = this->weight;
            SendRequest(request);
            waitingForTokens = true;
        }

        Request ReceiveRequest(){
            int message[2];
            MPI_Recv( message, 2, MPI_INT, leftNode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // TODO -> IRecv
            Request request = Request();
            request.priority = message[0];
            request.weight = message[1];
            return request;
        }

		void consumeTokens()
		{
			if (allRequests.empty()) {
				return;
			}
			Request current = allRequests.best();
			Request me = Request::make(0, this->weight, this->rank);

			while (current.weight <= numberOfTokens && !allRequests.empty()) {
				allRequests.erase(current);

				if (current == me) {
					SendPriorityIncrement(this->rank);
					startWorking();
					//Decreases numberOfTokens
				}
				else {
					numberOfTokens-= current.weight;
					SendTokens(current.weight);
				}

				if (sentRequests.contains(current)) {
					SendRelease(current);
					sentRequests.erase(current);
				}

				if (newRequests.contains(current)) {
					newRequests.erase(current);
				}

				current = allRequests.best();
			}
		}

		void acceptSentRequests(){
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

		void acceptSentReleases(){
			Request release = ReceiveRelease();
			while (release.correct) {
				if (sentRequests.contains(release)) {
					sentRequests.erase(release);
					allRequests.erase(release);
				}
				else {
					numberOfTokens += release.weight;
				}
			}
		}
		void acceptPriorityIncrement(){
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
		void Skier::acceptSentTokens(){
			int newTokens = ReceiveTokens();
			while (newTokens != -1){
				numberOfTokens += newTokens;
				newTokens = ReceiveTokens();
			}
		}
    public:
        Skier(int rank, int size, int tokens){
            /** srand(rank) -> repeatable weights, dependent upon rank  */
            srand(rank);
            this->rank = rank;
            priority = 0;
            numberOfTokens = tokens;
            weight = rand()%(MAX_WEIGHT - MIN_WEIGHT) + MIN_WEIGHT;

            /** create ring network */
            if(rank == size){
                leftNode = rank-1;
                rightNode = 0;
            } else if( rank == 0){
                leftNode = size;
                rightNode = rank+1;
            } else {
                leftNode = rank-1;
                rightNode = rank+1;
            }
        }

        ~Skier(){
            
        }

        int GetRank(){
            return rank;
        }

        void PrintNodes(){
            printf("left: %d; node: %d; right: %d; weight: %d tokens: %d\n",
                leftNode, rank, rightNode, weight, numberOfTokens
            );
        }

        void loop(){
            while (true) {
				consumeTokens();
				acceptSentRequests();
				acceptSentReleases();
				acceptSentTokens();
				std::cout<<"One revolution completed"<<std::endl;
				/*
				ImReadyController();
				SendRequestController();
				ReceivedRequestController();
				RecievedReleaseController();
				ReceivedTokensController();
				ImDoneController();
				*/
			}

		void startWorking(){
			numberOfTokens -= this->weight;
			Other things
			Start timer
		}

		bool isWorking(){
			return false;
		}

		
		void SendTokens(int tokens){

		}

		int ReceiveTokens(){
    		//If fail return -1
    		return -1;
		}
		void SendRelease(Request request){

		}

		Request ReceiveRelease(){
			Request r;
			// TODO
			//if(!succeded)
			//	{r.correct=false;}
			r.correct = false;
			return r;
		}

		void Skier::SendPriorityIncrement(int id){

		}

		int Skier::ReceivePriorityIncrement()
		{
			//returns -1 if no Received
			return -1;
		}
};

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
