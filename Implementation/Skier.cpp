#include "mpi.h"

#include <cstdio>
#include <cstdlib>
#include <list>

#include "Constants.h"

class Skier {
    struct Request {
        int priority;
        int weight;
    };
    private:
        std::list<Request> requests;
        //Skier* leftNode;
        //Skier* rightNode;
        int leftNode;
        int rightNode;
        int priority;
        int weight;
        int rank;

        bool waitingForTokens = false;
        
        int numberOfTokens;

        void SendRequest(int priority, int weight){
            Request request = Request();
            request.priority = priority;
            request.weight = weight;
            int message[2];
            message[0] = request.priority;
            message[1] = request.weight;
            MPI_Send( message, 2, MPI_INT, rightNode, 0, MPI_COMM_WORLD); // MPI_Isend > MPI_Send
        }

        void SendRequest(){
            SendRequest(this->priority, this->weight);
            waitingForTokens = true;
        }

        void ReceiveRequest(){
            int message[2];
            MPI_Recv( message, 2, MPI_INT, rightNode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // TODO -> IRecv
            Request request = Request();
            request.priority = message[0];
            request.weight = message[1];
            // TODO
        }

        void EnterLift(){
            waitingForTokens = false;
            numberOfTokens -= weight;
            printf("Skier %d entered the lift\n",rank);
            // TODO sleep
            printf("Skier %d left the lift\n",rank);
            numberOfTokens += weight;
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
            while(true){
                // TODO
            }
        }
        
};
