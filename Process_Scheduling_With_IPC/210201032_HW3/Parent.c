/*
HALÝL ÇAÐATAY ÇÝTKEN
210201032
OS HW-3
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

#define NO_OF_PROCESS 5
 

struct process {
	int processNo;
	long  workTime;
};
//this array holds estimated runtimes of processes
struct process processArrayEstimatedTime[NO_OF_PROCESS] = {
	{ 1,300 },
	{ 2,220 },
	{ 3,180 },
	{ 4,45 },
	{ 5,255 } };

//this array holds processes (n-1)th runtimes
struct process processArrayRealTime[NO_OF_PROCESS] = {
	{ 1,0 },
	{ 2,0 },
	{ 3,0 },
	{ 4,0 },
	{ 5,0 } };
//comparison function for estimated runtimes
int  compare(struct process *, struct process *);

int realRunTimes[5];
int main(int argc, char* argv[])
{
	//mutexes will hold processes from execution until parent allows them
	HANDLE mutexArray[NO_OF_PROCESS];
	//create mutexes for each process
	for (int i = 0; i < NO_OF_PROCESS; i++) {
		char m[5];
		_itoa(i+1, m, 10);
		mutexArray[i] = CreateMutex(NULL, FALSE, m);
		WaitForSingleObject(mutexArray[i], INFINITE);
	}

	long *runTime=0;/*real run time, will be collected from child while it is running*/
	DWORD dwordR;
	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];
	char* pipeNames[NO_OF_PROCESS] = { "\\\\.\\PIPE\\namedPipe1" , "\\\\.\\PIPE\\namedPipe2" , "\\\\.\\PIPE\\namedPipe3" , "\\\\.\\PIPE\\namedPipe4" , "\\\\.\\PIPE\\namedPipe5" };
	HANDLE pipeHandles[NO_OF_PROCESS];
	char* lpCommandLine[NO_OF_PROCESS] = { "child.exe 1","child.exe 2","child.exe 3","child.exe 4","child.exe 5" };

	// Named pipes are getting created
	for (int i = 0; i < NO_OF_PROCESS; i++) {
		pipeHandles[i] = CreateNamedPipe(pipeNames[i], PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 7000, 7000, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	}
	//Checking for pipe creaion errors
	for (int i = 0; i < NO_OF_PROCESS; i++) {
		if (pipeHandles[i] == INVALID_HANDLE_VALUE) {
			printf("Couldn't create %d. namedpipe.\n", i);
		}
	}
	//processes are getting created
	for (int i = 0; i < NO_OF_PROCESS; i++)
	{
		SecureZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		SecureZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));

		if (!CreateProcess(NULL,
			lpCommandLine[i],
			NULL,
			NULL,
			FALSE,
			CREATE_NO_WINDOW,
			NULL,
			NULL,
			&si[i],
			&pi[i]))
		{
			printf("unable to create process: %d\n", i);
			system("pause");
			ExitProcess(0);
		}
		else
		{
			processHandles[i] = pi[i].hProcess;
			//printf("parent has created process %d\n", i + 1);
		}


	}
	printf("\n");

	//getting pipe connections from child
	for (int i = 0; i < NO_OF_PROCESS; i++) {
		ConnectNamedPipe(pipeHandles[i], NULL);
	}

	for (int j = 0; j < NO_OF_PROCESS; j++) {
		qsort((void *)&processArrayEstimatedTime, 5, sizeof(struct process), compare);//sort the estimated values and print the next execution order
		printf("%d. Execution Order<P%d,P%d,P%d,P%d,P%d>\n",j+1,processArrayEstimatedTime[0].processNo, processArrayEstimatedTime[1].processNo, processArrayEstimatedTime[2].processNo, processArrayEstimatedTime[3].processNo, processArrayEstimatedTime[4].processNo);
		for (int i = 0; i < NO_OF_PROCESS; i++) {
			//free the particular mutex so process could enter it
			ReleaseMutex(mutexArray[processArrayEstimatedTime[i].processNo-1]);
			printf("P%d started.\n", processArrayEstimatedTime[i].processNo);
			//get its real run time from pipe
			if (!ReadFile(pipeHandles[processArrayEstimatedTime[i].processNo - 1], &runTime, 10, &dwordR, NULL)) {
				printf("Error: Couldn't get actual work time ...\n");
			}
			//calculate its prediction for the next step and put it in correct place
			for (int k = 0; k < NO_OF_PROCESS; k++) {
				if (processArrayEstimatedTime[i].processNo==processArrayRealTime[k].processNo) {
					processArrayRealTime[k].workTime = runTime;
					long temp = processArrayEstimatedTime->workTime;
					processArrayEstimatedTime[i].workTime = (0.5*processArrayRealTime[k].workTime) + (0.5)*temp;
				}
			}
			//lock the particular mutex until the next call
			printf("P%d ended.\n", processArrayEstimatedTime[i].processNo);
			WaitForSingleObject(mutexArray[processArrayEstimatedTime[i].processNo - 1], INFINITE);
		}
		printf("\n");
	}

	//waits until all the processes are finished
	WaitForMultipleObjects(NO_OF_PROCESS, processHandles, TRUE, INFINITE);

	//closing processes
	for (int i = 0; i < NO_OF_PROCESS; i++)
	{
		CloseHandle(pi[i].hProcess);
	}


	system("pause");
	return 1;
}
//comparison function for qsort()
int compare(struct process *p1, struct process *p2)
{
	if (p1->workTime < p2->workTime)
		return -1;
	else if (p1->workTime > p2->workTime)
		return 1;
	else
		return 0;
}
