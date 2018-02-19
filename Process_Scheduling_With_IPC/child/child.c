/*
HALÝL ÇAÐATAY ÇÝTKEN
210201032
OS HW-3
*/


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>


int main(int argc, char* argv[])
{
	//mutex will prevent this process from executing new tasks until parent releases the mutex
	HANDLE mutex = CreateMutex(NULL, FALSE, argv[1]);
	int processNo = atoi(argv[1]);
	srand(time(NULL)*processNo);//seed involves process no to prevent same random number generation by childs
	
	
	long *workTime;
	DWORD dwordW;
	char* pipeNames[5] = { "\\\\.\\PIPE\\namedPipe1" , "\\\\.\\PIPE\\namedPipe2" , "\\\\.\\PIPE\\namedPipe3" , "\\\\.\\PIPE\\namedPipe4" , "\\\\.\\PIPE\\namedPipe5" };
	HANDLE namedPipeHandle = INVALID_HANDLE_VALUE;

	namedPipeHandle = CreateFile(pipeNames[processNo-1], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	//checking pipe connection
	if (namedPipeHandle == INVALID_HANDLE_VALUE) {
		printf("Couldn't connect to a pipe...\n");
	}
	for (int i = 0; i < 5; i++) {
		//wait for mutex to be released
		WaitForSingleObject(mutex, INFINITE);

		workTime = rand() % 250 + 50;//generate random worktime 50-300 in ms
		Sleep(workTime);			//simulate the work by waiting
		//send the worktime to parent for calculation
		if (!WriteFile(namedPipeHandle, &workTime, 10, &dwordW, NULL)) {
			printf("Error: Couldn't send run time ...\n");
		}
		//release mutex so parent cant lock it again
		ReleaseMutex(mutex);
	}
	return 1;
}

