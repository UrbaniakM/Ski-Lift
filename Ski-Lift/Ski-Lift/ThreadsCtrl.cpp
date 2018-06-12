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

void receiveLeft(int node, std::mutex*omniMutex, std::mutex*myStartMutex, std::list<Request>& requestList, std::list<Request>& releaseList, std::list<int>& priorityList){
	myStartMutex->lock();
    while (true){
		int message[4];
		std::cout << "Recv::Ready()" << std::endl;
		MPI_Recv(message, 4, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (message[3] == RELEASE_MSG) {
            Request r(message[0],message[1],message[2]);
			omniMutex->lock();
			releaseList.push_front(r);
			omniMutex->unlock();
		}
		else if (message[3] == REQUEST_MSG) {
            Request r(message[0],message[1],message[2]);
			omniMutex->lock();
			requestList.push_front(r);
			omniMutex->unlock();
		}
		else if (message[3] == PRIORITY_MSG) {
			omniMutex->lock();
			priorityList.push_front(message[0]);
			omniMutex->unlock();
				//only write first
		}
	}
}

void receiveRight(int node, std::mutex*omniMutex, std::mutex*myStartMutex, std::list<int>& tokenList){
	myStartMutex->lock();
	while (true) {
		int message[1];
		std::cout << "Recv::Ready()" << std::endl;
		MPI_Recv(message, 1, MPI_INT, node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::cout << "Tokens recieved" << std::endl;
		omniMutex->lock();
		tokenList.push_front(message[0]);
		omniMutex->unlock();
	}
}

ThreadsCtrl*initializeThreads(int leftNode, int rightNode)
{
    ThreadsCtrl*threadsCtrl=new ThreadsCtrl();
    //threadsCtrl->receiveLeftThread = std::thread(receiveLeft, leftNode, &threadsCtrl->requestList, &threadsCtrl->releaseList, &threadsCtrl->priorityList);
    //threadsCtrl->receiveRightThread = std::thread(receiveRight, rightNode, &threadsCtrl->tokenList);
	threadsCtrl->startLeftMutex->lock();
	threadsCtrl->startRightMutex->lock();
	std::cout << "std::thread" << std::endl;
    std::thread receiveLeftThread(receiveLeft, leftNode, threadsCtrl->omniMutex, threadsCtrl->startLeftMutex, std::ref(threadsCtrl->requestList), std::ref(threadsCtrl->releaseList), std::ref(threadsCtrl->priorityList));
    std::thread receiveRightThread(receiveRight, rightNode, threadsCtrl->omniMutex, threadsCtrl->startRightMutex, std::ref(threadsCtrl->tokenList));

	std::cout << "std::thread::end" << std::endl; 
	return threadsCtrl;
}
