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
#include "PipeCtrl.h"


PipeCtrl forkpipe(int leftnode, int rightnode)
{
	PipeCtrl pipeCtrl;

	int fd0[2];  // Tokens
	int fd1[2];  // Release
	int fd2[2];  // Request
	int fd3[2];  // Priority

	pid_t p;

	if (pipe(fd0) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		exit(-1);
	}
	if (pipe(fd1) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		exit(-1);
	}
	if (pipe(fd2) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		exit(-1);
	}
	if (pipe(fd3) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		exit(-1);
	}
	p = fork();

	if (p < 0)
	{
		fprintf(stderr, "fork Failed");
		exit(-1);
	}

	// Parent process
	else if (p > 0)
	{
		PipeCtrl pc(fd0[1], fd1[1], fd2[1], fd3[1]);

		close(fd0[0]);
		close(fd1[0]);
		close(fd2[0]);
		close(fd3[0]);
		return pc;
	}

	// child process
	else
	{
		int tokpipe = fd0[0];
		int relpipe = fd1[0];
		int reqpipe = fd2[0];
		int pripipe = fd3[0];
		close(fd0[1]);
		close(fd1[1]);
		close(fd2[1]);
		close(fd3[1]);
		if (fork() == 0) {
			while (true){
				int message[4];
				MPI_Barrier(MPI_COMM_WORLD);
				MPI_Recv(message, 4, MPI_INT, leftnode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if (message[3] == RELEASE_MSG) {
					write(relpipe, (char*)message, sizeof(int) * 3);
				}
				else if (message[3] == REQUEST_MSG) {
					write(reqpipe, (char*)message, sizeof(int) * 3);
				}
				else if (message[3] == PRIORITY_MSG) {
					write(pripipe, (char*)message, sizeof(int) );
						//only write first
				}
			}
		}
		else {
			while (true) {
				int message[1];
				MPI_Barrier(MPI_COMM_WORLD);
				MPI_Recv(message, 1, MPI_INT, rightnode, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				std::cout << "Tokens recieved" << std::endl;
				write(tokpipe, (char*)message, sizeof(int));
			}
		};
	}
}
