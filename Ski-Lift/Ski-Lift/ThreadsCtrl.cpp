#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<iostream>
#include<sys/wait.h>
#include "mpi.h"
#include "main.h"
#include "Request.h"
#include "ThreadsCtrl.h"
#include <thread>
#include <mutex>
#include <list>

void receiveLeft(int node, std::list<Request>& requestList, std::list<Request>& releaseList, std::list<int>& priorityList){
    while (true){
		int message[4];
		MPI_Recv(message, 4, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (message[3] == RELEASE_MSG) {
            Request r(message[0],message[1],message[2]);
			releaseList.push_front(r);
		}
		else if (message[3] == REQUEST_MSG) {
            Request r(message[0],message[1],message[2]);
			requestList.push_front(r);
		}
		else if (message[3] == PRIORITY_MSG) {
			priorityList.push_front(message[0]);
				//only write first
		}
	}
}

void receiveRight(int node, std::list<int>& tokenList){
    while (true) {
		int message[1];
		MPI_Recv(message, 1, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::cout << "Tokens recieved" << std::endl;
		tokenList.push_front(message[0]);
	}
}

ThreadsCtrl initializeThreads(int leftNode, int rightNode)
{
    ThreadsCtrl threadsCtrl;
    //threadsCtrl.receiveLeftThread = std::thread(receiveLeft, leftNode, &threadsCtrl.requestList, &threadsCtrl.releaseList, &threadsCtrl.priorityList);
    //threadsCtrl.receiveRightThread = std::thread(receiveRight, rightNode, &threadsCtrl.tokenList);
    std::thread receiveLeftThread(receiveLeft, leftNode, std::ref(threadsCtrl.requestList), std::ref(threadsCtrl.releaseList), std::ref(threadsCtrl.priorityList));
    std::thread receiveRightThread(receiveRight, rightNode, std::ref(threadsCtrl.tokenList));
    return threadsCtrl;
}
