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
#include <pthread.h>
#include <mutex>
#include <list>

class MyMutex {
	pthread_mutex_t  mutex;
public:
	MyMutex() {
		pthread_mutex_init(mutex, NULL);
	}
	~MyMutex() {
		pthread_mutex_destroy(mutex);
	}
	void lock() {
		pthread_mutex_lock(mutex);
	}
	void unlock() {
		pthread_mutex_unlock(mutex);
	}
};

struct ThreadsCtrl {
	ThreadsCtrl() {
		omniMutex = new MyMutex();
		startRightMutex = new MyMutex();
		startLeftMutex = new MyMutex();
	}
	~ThreadsCtrl() {
		delete omniMutex;
		delete startRightMutex;
		delete startLeftMutex;
	}

    std::list<int> tokenList;
    std::list<Request> requestList;
    std::list<Request> releaseList;
    std::list<int> priorityList;

    // TODO: transform struct into class, declare this mutexes as global with 'extern' in header file

    //MyMutex _mutexTokens;
    //MyMutex mutexRequests;
    //MyMutex mutexReleases;
    //MyMutex mutexPriorities;
	MyMutex*omniMutex;
	MyMutex*startRightMutex;
	MyMutex*startLeftMutex;

	void sendTokens(int tokens, int node) {
		int arr[1] = { tokens };
		MPI_Send(arr, 1, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	int readTokens() {
		std::cout << "Reciving_tokens_REEEAL" << std::endl;
        //std::lock_guard<MyMutex> lock(mutexTokens);
		omniMutex->lock();
		if (tokenList.empty()) {
			omniMutex->unlock();
			return -1;
			std::cout << "Tokens fail" << std::endl;
		}
        int token = tokenList.front();
        tokenList.pop_front();
		omniMutex->unlock();
        return token;
		std::cout << "Tokens succes" << std::endl;
	}
	void sendRequest(int priority, int weight, int id, int node) {
		int arr[4] = { priority, weight , id , REQUEST_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	Request readRequest() {
        Request r;
		omniMutex->lock();
		if(!requestList.empty()){
            r = requestList.front();
            requestList.pop_front();
        } else {
			r.correct = false;
		}
		omniMutex->unlock();
		return r;
	}
	void sendRelease(int priority, int weight, int id, int node) {
		int arr[4] = { priority, weight , id , RELEASE_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	Request readRelease() {
		Request r;
		omniMutex->lock();
		if(!releaseList.empty()){
            r = releaseList.front();
            releaseList.pop_front();
        }
		else {
			r.correct = false;
		}
		omniMutex->unlock();
		return r;
	}
	void sendPriorityIncrement(int id, int node) {
		int arr[4] = { id, -1 , -1 , PRIORITY_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	int readPriorityIncrement() {
		omniMutex->lock();
		if (priorityList.empty()) {
			omniMutex->unlock();
			return -1;
		}
        int priority = priorityList.front();
        priorityList.pop_front();
		omniMutex->unlock();
		return priority;
	}
};


ThreadsCtrl*initializeThreads(int leftnode, int rightnode);
