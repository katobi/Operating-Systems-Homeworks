/*
HALÝL ÇAÐATAY ÇÝTKEN
210201032
OS HW-1
*/


//FYI: Some of the code including opening threads and processes are from our thread lab codes.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>




typedef struct
{
	char* itemType;
	int threadNo;
}THREAD_PARAMETERS;

DWORD WINAPI threadFunction(LPVOID parameters);

char assignedDay[30];
//below two strings will be completed once pipe gives a day
char startDay[20] = "#START DAY ";
char endDay[20] = "#END DAY ";
int countArray[4] = { 0,0,0,0 };//count[0] is for milk
								//		1  is for biscuit
								//      2  is for chips
								//      3 is for coke



int main(int argc, char* argv[])
{
	char* daysInWeek[7] = { "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY" };
	char* itemsToRead[4] = { "MILK","BISCUIT","CHIPS","COKE" };
	DWORD dwordR;
	DWORD dwordW;


	int assignedDayNo;
	char* pipeNames[7] = { "\\\\.\\PIPE\\namedPipe1" , "\\\\.\\PIPE\\namedPipe2" , "\\\\.\\PIPE\\namedPipe3" , "\\\\.\\PIPE\\namedPipe4" , "\\\\.\\PIPE\\namedPipe5" , "\\\\.\\PIPE\\namedPipe6" , "\\\\.\\PIPE\\namedPipe7" };
	HANDLE namedPipeHandle = INVALID_HANDLE_VALUE;
	HANDLE* handles;
	THREAD_PARAMETERS* lpParameter;
	int* threadID;
	int i = 0;
	int threadCount = 0;


	//childs will try to connect any pipe they can connect then break out the loop
	for (i = 0; i < 7; i++) {
		if (namedPipeHandle == INVALID_HANDLE_VALUE) {
			namedPipeHandle = CreateFile(pipeNames[i], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		}
		else break;
	}

	//checking pipe connection
	if (namedPipeHandle == INVALID_HANDLE_VALUE) {
		printf("Couldn't connect to a pipe...\n");
	}


	//reading day name from parent process
	if (!ReadFile(namedPipeHandle, assignedDay, 100, &dwordR, NULL)) {
		printf("Couldn't get assigned to a day by parent...\n");
	}

	//assigning a day number from week for eg. if day is monday dayno is 1
	for (i = 0; i < 7; i++) {
		if (strcmp(assignedDay, daysInWeek[i]) == 0) {
			assignedDayNo = i + 1;
		}
	}
	//this is for completing #START DAY X# string
	for (i = 0; i < 7; i++) {
		if (assignedDayNo == i + 1) {

			char str[4];
			sprintf(str, "%d", i + 1);
			strcat(str, "#");
			strcat(startDay, str);
			strcat(endDay, str);

		}
	}
	//most of the codes below are from tread lab solution i just tweaked them a bit
	if (argc != 2)
	{
		printf("error in child process...now exiting %d\n", argv[0]);
		system("pause");
		exit(0);
	}


	//read thread count from arguments
	threadCount = atoi(argv[1]);


	//allocate memory for every parameters needed
	handles = malloc(sizeof(HANDLE)* threadCount);
	lpParameter = malloc(sizeof(THREAD_PARAMETERS)* threadCount);
	threadID = malloc(sizeof(int)* threadCount);

	//for each thread
	for (i = 0; i < threadCount; i++)
	{
		//initialize parameters
		lpParameter[i].threadNo = i;
		//assigns thread to a certain item
		lpParameter[i].itemType = itemsToRead[i];
		handles[i] = CreateThread(NULL, 0, threadFunction, &lpParameter[i], 0, &threadID[i]);


		//check errors in creation
		if (handles[i] == INVALID_HANDLE_VALUE)
		{
			printf("error when creating thread\n");
			system("pause");
			exit(0);
		}
	}

	//waits for threads to end
	WaitForMultipleObjects(threadCount, handles, TRUE, INFINITE);


	//sends the count numbers to parent process via named pipe
	if (!WriteFile(namedPipeHandle, countArray, 50, &dwordW, NULL)) {
		printf("Error: Couldn't send count numbers ...\n");
	}

	//closes threads
	for (i = 0; i < threadCount; i++)
	{
		CloseHandle(handles[i]);
	}


	free(handles);
	free(lpParameter);
	free(threadID);

	return 1;
}


// 4 Thread Function will be created and they all will work with the function below
// However they will have different parameter so they will read different items
DWORD WINAPI threadFunction(LPVOID parameters)
{
	//initating threads parameter
	THREAD_PARAMETERS* param = (THREAD_PARAMETERS*)parameters;
	FILE* file = NULL;
	char *p = ""; //for parse
	char line[200];

	//open the file for reading
	file = fopen("market.txt", "rt");

	//control if file couldnt be opened
	if (file == NULL) {
		printf("Couldn't open the file...\n");
	}
	while (fgets(line, 200, file) != NULL)
	{
		if (strstr(line, startDay) != NULL) {
			fgets(line, 200, file);
			while (strstr(line, endDay) == NULL) {
				//parse the line with comma and \n
				p = strtok(line, ",\n");
				while (p != NULL)
				{
					//if parsed substring has parameter item type increment its count value
					if (strstr(p, param->itemType) != NULL) {
						countArray[param->threadNo]++;
					}
					p = strtok(NULL, ",\n");
				}
				//jump to next line
				fgets(line, 200, file);
			}
		}
	}
	//close the file
	fclose(file);
	return 1;
}


