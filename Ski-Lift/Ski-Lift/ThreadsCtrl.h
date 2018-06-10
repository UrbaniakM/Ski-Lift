#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<iostream>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include "mpi.h"
#include "main.h"
#include "ThreadsCtrl.h"
#include "Request.h"
#include <thread>
#include <mutex>
#include <list>


struct ThreadsCtrl {
	ThreadsCtrl() {}
    //std::thread receiveLeftThread;
    //std::thread receiveRightThread;

    std::list<int> tokenList;
    std::list<Request> requestList;
    std::list<Request> releaseList;
    std::list<int> priorityList;
    

	void sendTokens(int tokens, int node) {
		int arr[1] = { tokens };
		MPI_Send(arr, 1, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	int readTokens() {
		std::cout << "Reciving_tokens_REEEAL" << std::endl;
		if (tokenList.empty()) {
			return -1;
			std::cout << "Tokens fail" << std::endl;
		}
        int token = tokenList.front();
        tokenList.pop_front();
        return token;
		std::cout << "Tokens succes" << std::endl;
	}
	void sendRequest(int priority, int weight, int id, int node) {
		int arr[4] = { priority, weight , id , REQUEST_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	Request readRequest() {
        Request r;
		if(!requestList.empty()){
            r = requestList.front();
            requestList.pop_front();
        } else {
			r.correct = false;
		}
		return r;
	}
	void sendRelease(int priority, int weight, int id, int node) {
		int arr[4] = { priority, weight , id , RELEASE_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	Request readRelease() {
		Request r;
		if(!releaseList.empty()){
            r = releaseList.front();
            releaseList.pop_front();
        }
		else {
			r.correct = false;
		}
		return r;
	}
	void sendPriorityIncrement(int id, int node) {
		int arr[4] = { id, -1 , -1 , PRIORITY_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	int readPriorityIncrement() {
		if (priorityList.empty()) {
			return -1;
		}
        int priority = priorityList.front();
        priorityList.pop_front();
		return priority;
	}
};


ThreadsCtrl initializeThreads(int leftnode, int rightnode);
