#include "Request.h"
#include "mpi.h"

// MESSAGE TAGS
#define TOKENS_TAG 0
#define RELEASE_TAG 1
#define REQUEST_TAG 2
#define PRIORITY_TAG 3
#define CANCEL_REQUEST 4

class MPIManager {
    public: 
        MPIManager();
        inline void RightNode(int rightNode){
            this->rightNode = rightNode;
        }
        inline void LeftNode(int leftNode){
            this->leftNode = leftNode;
        }

        void SendRequest(Request request);
        void SendRelease(Request request);
        void SendPriorityIncrement(PriorityIncrement priorityIncrement);
        void SendTokens(int tokens);
        void SendCancelRequest(int id);

        Request ReceiveRequest();
        Request ReceiveRelease();
        PriorityIncrement ReceivePriorityIncrement();
        int ReceiveTokens();
        int ReceiveCancelRequest();
    
    private:
        int currentNode;
        int rightNode;
        int leftNode;

        bool triedReceiveLeftRequest;
        bool triedReceiveLeftRelease;
        bool triedReceiveLeftPriority;
        bool triedReceiveRightTokens;
        bool triedReceiveLeftCancelRequest;
        MPI_Request leftReceiveRequest;
        MPI_Request leftReceiveRelease;
        MPI_Request leftReceivePriority;
        MPI_Request rightReceiveTokens;
        MPI_Request leftReceiveCancelRequest;
        int leftBufferRequest[3];
        int leftBufferRelease[3];
        int leftBufferPriority[2];
        int rightBufferTokens[1];
        int leftBufferCancelRequest[1];
};
