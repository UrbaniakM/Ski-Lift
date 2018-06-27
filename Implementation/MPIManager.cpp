#include "MPIManager.h"

MPIManager::MPIManager(){
    triedReceiveLeftRequest = false;
    triedReceiveLeftPriority = false;
    triedReceiveRightTokens = false;
    triedReceiveLeftCancelRequest = false;
}

///
/// Sends Request (3 ints - priority, weight and id) to the Right Node.
/// MPI Communication
/// Type: Send
/// Message tag: REQUEST_TAG
/// Node: rightNode
/// Buffer size: 3
/// Buffer datatype: MPI_INT
///
void MPIManager::SendRequest(Request request)
{
    int arr[3] = { request.priority, request.weight, request.id};
	MPI_Send(arr, 3, MPI_INT, rightNode, REQUEST_TAG, MPI_COMM_WORLD);
}

///
/// Send Priority Increment Id (1 int) to the Right Node.
/// MPI Communication
/// Type: Send
/// Message tag: PRIORITY_TAG
/// Node: leftNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
void MPIManager::SendPriorityIncrement(int id)
{
    int arr[1] = { id };
	MPI_Send(arr, 1, MPI_INT, rightNode, PRIORITY_TAG, MPI_COMM_WORLD);
}

///
/// Sends Tokens (1 int) to the Left Node.
/// MPI Communication
/// Type: Send
/// Message tag: TOKENS_TAG
/// Node: leftNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
void MPIManager::SendTokens(TokensStruct tokens){
    int arr[2] = { tokens.tokens, tokens.target };
    MPI_Send(arr, 2, MPI_INT, leftNode, TOKENS_TAG, MPI_COMM_WORLD);
}

///
/// Sends Id (1 int) of the Node to the Right Node to inform about cancellation of its request.
/// MPI Communication
/// Type: Send
/// Message tag: CANCEL_REQUEST
/// Node: rightNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
void MPIManager::SendCancelRequest(int id){
    int arr[1] = { id };
    MPI_Send(arr, 1, MPI_INT, rightNode, CANCEL_REQUEST, MPI_COMM_WORLD);
}

///
/// Receives Request (3 ints - priority, weight and id) from Left Node if encounters in buffer, otherwise request is marked as incorrect. 
/// MPI Communication
/// Type: IRecv
/// Message tag: REQUEST_TAG
/// Node: leftNode
/// Buffer size: 3
/// Buffer datatype: MPI_INT
///
Request MPIManager::ReceiveRequest()
{
    Request r;
    r.correct = false;
    if(triedReceiveLeftRequest){
        int flag;
        MPI_Request_get_status(leftReceiveRequest,&flag, MPI_STATUS_IGNORE);
        if(flag){
            r.priority = leftBufferRequest[0];
            r.weight = leftBufferRequest[1];
            r.id = leftBufferRequest[2];
            r.correct = true;
            triedReceiveLeftRequest = false;
        }
    }
    else {
        MPI_Irecv(leftBufferRequest, 3, MPI_INT, leftNode, REQUEST_TAG, MPI_COMM_WORLD, &leftReceiveRequest);
        triedReceiveLeftRequest = true;
    }
	return r;
}

///
/// Receives Priority Increment by Id (1 int) from Left Node if encounters in buffer, otherwise returns -1. 
/// MPI Communication
/// Type: IRecv
/// Message tag: PRIORITY_TAG
/// Node: rightNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
int MPIManager::ReceivePriorityIncrement()
{
    int id = -1;
    if(triedReceiveLeftPriority){
        int flag;
        MPI_Request_get_status(leftReceivePriority,&flag, MPI_STATUS_IGNORE);
        if(flag){
            id = leftBufferPriority[0];
            triedReceiveLeftPriority = false;
        }
    }
    else {
        MPI_Irecv(leftBufferPriority, 1, MPI_INT, leftNode, PRIORITY_TAG, MPI_COMM_WORLD, &leftReceivePriority);
        triedReceiveLeftPriority = true;
    }
    return id;
}


///
/// Receives Tokens (1 int) from Right Node if encounters in buffer, otherwise returns 0.
/// MPI Communication
/// Type: IRecv
/// Message tag: TOKENS_TAG
/// Node: rightNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
TokensStruct MPIManager::ReceiveTokens()
{
    TokensStruct tokens;
    tokens.correct = false;
    if(triedReceiveRightTokens){
        int flag;
        MPI_Request_get_status(rightReceiveTokens,&flag, MPI_STATUS_IGNORE);
        if(flag){
            tokens.tokens = rightBufferTokens[0];
            tokens.target = rightBufferTokens[1];
            tokens.correct = true;
            triedReceiveRightTokens = false;
        }
    }
    else {
        MPI_Irecv(rightBufferTokens, 2, MPI_INT, rightNode, TOKENS_TAG, MPI_COMM_WORLD, &rightReceiveTokens);
        triedReceiveRightTokens = true;
        
    }
    return tokens;
}

///
/// Receives Id (1 int) from Left Node if encounters in buffer, otherwise returns -1. 
/// MPI Communication
/// Type: IRecv
/// Message tag: CANCEL_REQUEST
/// Node: leftNode
/// Buffer size: 1
/// Buffer datatype: MPI_INT
///
int MPIManager::ReceiveCancelRequest()
{
    int id = -1;
    if(triedReceiveLeftCancelRequest){
        int flag;
        MPI_Request_get_status(leftReceiveCancelRequest,&flag, MPI_STATUS_IGNORE);
        if(flag){
            id = leftBufferCancelRequest[0];
            triedReceiveLeftCancelRequest = false;
        }
    }
    else {
        MPI_Irecv(leftBufferCancelRequest, 1, MPI_INT, leftNode, CANCEL_REQUEST, MPI_COMM_WORLD, &leftReceiveCancelRequest);
        triedReceiveLeftCancelRequest = true;
        
    }
    return id;
}