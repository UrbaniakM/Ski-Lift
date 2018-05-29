#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include "mpi.h"
#include <fcntl.h>
#include "main.h"
#include "PipeCtrl.h"
#include "Request.h"



struct PipeCtrl {
	PipeCtrl() {}
	PipeCtrl(int t, int rl, int rq, int pp) {
		tokenpipe = t;
		releasepipe = rl;
		requestpipe = rq;
		prioritypipe = pp;
		if (fcntl(t, F_SETFL, O_NONBLOCK) < 0)
		{
			exit(-1);
		}
		if (fcntl(rl, F_SETFL, O_NONBLOCK) < 0)
		{
			exit(-1);
		}
		if (fcntl(rq, F_SETFL, O_NONBLOCK) < 0)
		{
			exit(-1);
		}
		if (fcntl(pp, F_SETFL, O_NONBLOCK) < 0)
		{
			exit(-1);
		}
	}
	int tokenpipe;
	int requestpipe;
	int releasepipe;
	int prioritypipe;
	void sendTokens(int tokens, int node) {
		int arr[1] = { tokens };
		MPI_Send(arr, 1, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	int readTokens() {
		int arr[1];
		int rd = read(tokenpipe, arr, sizeof(int));
		if (rd == -1) {
			return -1;
		}
		return arr[0];
	}
	void sendRequest(int priority, int weight, int id, int node) {
		int arr[4] = { priority, weight , id , REQUEST_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	Request readRequest() {
		int arr[3];
		int rd=read(requestpipe, arr, sizeof(int) * 3);
		Request r(arr[0], arr[1], arr[2]);
		if (rd == -1) {
			r.correct = false;
		}
		return r;
	}
	void sendRelease(int priority, int weight, int id, int node) {
		int arr[4] = { priority, weight , id , RELEASE_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	Request readRelease() {
		int arr[3];
		int rd = read(releasepipe, arr, sizeof(int) * 3);
		Request r(arr[0], arr[1], arr[2]);
		if (rd == -1) {
			r.correct = false;
		}
		return r;
	}
	void sendPriorityIncrement(int id, int node) {
		int arr[4] = { id, -1 , -1 , PRIORITY_MSG };
		MPI_Send(arr, 4, MPI_INT, node, 0, MPI_COMM_WORLD);
	}
	int readPriorityIncrement() {
		int*arr;
		int rd=read(prioritypipe, arr, sizeof(int));
		if (rd == -1) {
			return -1;
		}
		return *arr;
	}
};


PipeCtrl forkpipe(int leftnode, int rightnode);