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

        void SendRequest(){
            Request request = Request();
            request.priority = this->priority;
            request.weight = this->weight;
            //MPI_Send( PRIORITY );
            //MPI_Send( WEIGHT );
            // TODO
        }

        void ReceiveRequest(){
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
