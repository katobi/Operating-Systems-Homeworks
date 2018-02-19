/*
HALÝL ÇAÐATAY ÇÝTKEN
210201032
OS HW-1
*/

//FYI: Some of the code including opening threads and processes are from our thread lab codes.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

#define NO_OF_PROCESS 7

struct products {
	char* productName;
	int count;
};
//this structure will hold every information
// first index is for the day number
// second index is for a particular item
// for instance allData[0][0] means monday and milk count
int allData[7][4];
//declarations
struct products mostSoldItemInWeek();
void compare4(int pmilk, int pbiscuit, int pchips, int pcoke);
void mostSoldtemEachDay();
void totalNumberOfEachItemSoldInWeek();
void totalNumerOfEachItemEachDay();
int main(int argc, char* argv[])
{
	DWORD dwordW;
	DWORD dwordR;
	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];
	char* daysInWeek[7] = { "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY" };
	char* pipeNames[7] = { "\\\\.\\PIPE\\namedPipe1" , "\\\\.\\PIPE\\namedPipe2" , "\\\\.\\PIPE\\namedPipe3" , "\\\\.\\PIPE\\namedPipe4" , "\\\\.\\PIPE\\namedPipe5" , "\\\\.\\PIPE\\namedPipe6" , "\\\\.\\PIPE\\namedPipe7" };
	HANDLE pipeHandles[7];

	char* lpCommandLine[NO_OF_PROCESS] = { "child.exe 4","child.exe 4","child.exe 4","child.exe 4","child.exe 4","child.exe 4","child.exe 4" };
	int i = 0;
	// Named pipes are getting created
	for (i = 0; i < 7; i++) {
		pipeHandles[i] = CreateNamedPipe(pipeNames[i], PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, 7000, 7000, NMPWAIT_USE_DEFAULT_WAIT, NULL);
	}

	//Checking for pipe creaion errors
	for (i = 0; i < 7; i++) {
		if (pipeHandles[i] == INVALID_HANDLE_VALUE) {
			printf("Couldn't create %d. namedpipe.\n", i);
		}
	}


	//processes are getting created

	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		SecureZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		SecureZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));

		if (!CreateProcess(NULL,
			lpCommandLine[i],
			NULL,
			NULL,
			FALSE,
			CREATE_NO_WINDOW, //CREATE_NEW_CONSOLE
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
			printf("parent has created process number %d\n", i);
		}


	}


	//getting connections from child
	for (i = 0; i < 7; i++) {
		ConnectNamedPipe(pipeHandles[i], NULL);
	}
	//assigning days via pipes
	for (i = 0; i < 7; i++) {
		if (!WriteFile(pipeHandles[i], daysInWeek[i], 25, &dwordW, NULL)) {
			printf("Error: Couldn't send day information %d...\n", i);
		}
	}









	//waits until all the processes are finished
	WaitForMultipleObjects(NO_OF_PROCESS, processHandles, TRUE, INFINITE);

	//getting and assigning count data from childs
	for (i = 0; i < 7; i++) {
		int countArray[50];
		if (!ReadFile(pipeHandles[i], countArray, 200, &dwordR, NULL)) {
			printf("Error: Couldn't get days counts ...\n");
		}
		else {
			for (int k = 0; k < 4; k++) {
				allData[i][k] = countArray[k];
			}
		}
	}

	//homework functions
	printf("MOST SOLD ITEM IN WEEK:%s %d\n", mostSoldItemInWeek().productName, mostSoldItemInWeek().count);
	mostSoldtemEachDay();
	totalNumberOfEachItemSoldInWeek();
	totalNumerOfEachItemEachDay();
	//closing processes
	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}

	printf("\n");
	system("pause");
	return 1;
}



// This function will return a structure after comparing sold items
struct products mostSoldItemInWeek() {
	int milkTotal = 0;
	int biscuitTotal = 0;
	int chipsTotal = 0;
	int cokeTotal = 0;
	//Getting total numbers of items
	for (int i = 0; i < 7; i++) {
		milkTotal = milkTotal + allData[i][0];
	}

	for (int i = 0; i < 7; i++) {
		biscuitTotal = biscuitTotal + allData[i][1];
	}
	for (int i = 0; i < 7; i++) {
		chipsTotal = chipsTotal + allData[i][2];
	}
	for (int i = 0; i < 7; i++) {
		cokeTotal = cokeTotal + allData[i][3];
	}
	//Initiating structures
	struct products max;
	struct products milk;
	milk.count = milkTotal;
	milk.productName = "MILK";
	struct products biscuit;
	biscuit.count = biscuitTotal;
	biscuit.productName = "BISCUIT";
	struct products chips;
	chips.count = chipsTotal;
	chips.productName = "CHIPS";
	struct products coke;
	coke.count = cokeTotal;
	coke.productName = "COKE";
	//comparisonbetween the items
	max.count = milk.count;
	max.productName = milk.productName;

	if (max.count<biscuit.count) {
		max = biscuit;
	}
	if (max.count<chips.count) {
		max = chips;
	}
	if (max.count<coke.count) {
		max = coke;
	}
	//returns the strucure
	return max;
}


void mostSoldtemEachDay() {

	for (int i = 0; i < 7; i++) {
		char day[30] = "Most Sold Item In Day ";
		char str[3];
		sprintf(str, "%d", i + 1);
		strcat(day, str);
		printf("%s:  ", day);
		compare4(allData[i][0], allData[i][1], allData[i][2], allData[i][3]);
	}
}

// will print each items total number in week
void totalNumberOfEachItemSoldInWeek() {
	int totalMilk = 0;
	int totalBiscuit = 0;
	int totalChips = 0;
	int totalCoke = 0;

	for (int i = 0; i < 7; i++) {
		totalMilk = totalMilk + allData[i][0];
	}
	for (int i = 0; i < 7; i++) {
		totalBiscuit = totalBiscuit + allData[i][1];
	}
	for (int i = 0; i < 7; i++) {
		totalChips = totalChips + allData[i][2];
	}
	for (int i = 0; i < 7; i++) {
		totalCoke = totalCoke + allData[i][3];
	}

	printf("Total Milk Sold In Week: %d\nTotal Biscuit Sold In Week: %d\nTotal Chips Sold In Week: %d\nTotal Coke Sold In Week: %d\n", totalMilk, totalBiscuit, totalChips, totalCoke);
}

// This function will compare 4 given numbers and prints the maxes of them
void compare4(int pmilk, int pbiscuit, int pchips, int pcoke) {

	struct products max;
	struct products milk;
	milk.count = pmilk;
	milk.productName = "MILK";
	struct products biscuit;
	biscuit.count = pbiscuit;
	biscuit.productName = "BISCUIT";
	struct products chips;
	chips.count = pchips;
	chips.productName = "CHIPS";
	struct products coke;
	coke.count = pcoke;
	coke.productName = "COKE";

	max.count = milk.count;
	max.productName = milk.productName;

	if (max.count <= biscuit.count) {
		max = biscuit;
	}
	if (max.count <= chips.count) {
		max = chips;
	}
	if (max.count <= coke.count) {
		max = coke;
	}
	printf("%d %s", max.count, max.productName);
	if (strcmp(max.productName, milk.productName) != 0 && max.count == milk.count) { printf(" %d %s", milk.count, milk.productName); }
	if (strcmp(max.productName, biscuit.productName) != 0 && max.count == biscuit.count) { printf(" %d %s", biscuit.count, biscuit.productName); }
	if (strcmp(max.productName, chips.productName) != 0 && max.count == chips.count) { printf(" %d %s", chips.count, chips.productName); }
	printf("\n");
}

// will print each item count for each day
void totalNumerOfEachItemEachDay() {

	for (int i = 0; i < 7; i++) {
		char day[30] = "In Day ";
		char str[3];
		sprintf(str, "%d", i + 1);
		strcat(day, str);
		printf("%s:    ", day);
		printf("MILK:%d    BISCUIT:%d   CHIPS:%d   COKE:%d\n", allData[i][0], allData[i][1], allData[i][2], allData[i][3]);

	}

}

