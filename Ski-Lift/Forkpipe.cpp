// C program to demonstrate use of fork() and pipe()
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include "mpi.h"

struct PipeCtrl {
	PipeCtrl(){}
	PipeCtrl(int s, int r) {
		send = s;
		recv = r;
	}
	int send;
	int recv;
};

PipeCtrl forkpipe()
{
	PipeCtrl pipeCtrl;

	int fd1[2];  // Used to store two ends of first pipe
	int fd2[2];  // Used to store two ends of second pipe

	char fixed_str[] = "forgeeks.org";
	char input_str[100];
	pid_t p;

	if (pipe(fd1) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		return 1;
	}
	if (pipe(fd2) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		return 1;
	}

	scanf("%s", input_str);
	p = fork();

	if (p < 0)
	{
		fprintf(stderr, "fork Failed");
		return 1;
	}

	// Parent process
	else if (p > 0)
	{
		char concat_str[100];

		close(fd1[0]);
		close(fd2[0]);
		return 
	}

	// child process
	else
	{
		close(fd1[1]);  // Close writing end of first pipe

						// Read a string using first pipe
		char concat_str[100];
		read(fd1[0], concat_str, 100);

		// Concatenate a fixed string with it
		int k = strlen(concat_str);
		int i;
		for (i = 0; i<strlen(fixed_str); i++)
			concat_str[k++] = fixed_str[i];

		concat_str[k] = '\0';   // string ends with '\0'

								// Close both reading ends
		close(fd1[0]);
		close(fd2[0]);

		// Write concatenated string and close writing end
		write(fd2[1], concat_str, strlen(concat_str) + 1);
		close(fd2[1]);

		exit(0);
	}
}