#include "mpi.h"

#include <stdio.h>
#include <list>

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
        
        int currentNumberOfTokens = 0;

        void SendRequest(int priority, int weight){
            Request request = Request();
            request.priority = priority;
            request.weight = weight;
            int message[2];
            message[0] = request.priority;
            message[1] = request.weight;
            MPI_Send( message, 2, MPI_INT, rightNode, 0, MPI_COMM_WORLD);
        }

        void SendRequest(){
            SendRequest(this->priority, this->weight);
        }

        void ReceiveRequest(){
            int message[2];
            MPI_Recv( message, 2, MPI_INT, rightNode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            Request request = Request();
            request.priority = message[0];
            request.weight = message[1];
            // TODO
        }
    public:
        Skier(int rank, int size){
            this->rank = rank;
            priority = 0;
            // TODO: rand weight

            // create ring network
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
            printf("left: %d; node: %d; right: %d\n",leftNode, rank, rightNode);
        }

        void loop(){
            while(true){
                // TODO
            }
        }
        
};
