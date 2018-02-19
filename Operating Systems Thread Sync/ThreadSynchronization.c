/*
HALİL ÇAĞATAY ÇİTKEN
210201032
OS_HW_2
*/
//FYI: program has no sleep function, when it deletes a file, it creates a copy and switches that copy with original file.
//when this operation gets done without sleep the OS cant catch up and there may be some leftover empty copy.txt files. 
//However this wont effect the program at all.Other than that the program works totally fine.

#include <unistd.h>     
#include <sys/types.h>   
#include <errno.h>     
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <string.h> 
#include <semaphore.h> 
#include <time.h>

int currentRunningThreads=0;// number of threads that is currently running in the system
int thread_exit_1;//int variable for threads to exit upon
int isResource1Empty=0;//it will become 1 if the resource 1 is empty
int isResource2Empty=0;//it will become 1 if the resource 2 is empty
int turn=0;// will be shared between Thread A type threads

#define RESOURCE1 "numbers1.txt"
#define RESOURCE2 "numbers2.txt"
#define RESOURCE3 "numbers3.txt"

void A (void *ptr);//A type threads will use this function
void B (void *ptr);//B type thread will use this function
void C (void *ptr);//C type thread will use this function
int is_prime(int num);//prime control function returns 1 if prime, 0 if not
void deletePrimes(char* fileName);//deletes all the prime numbers from given file
void deleteNegatives(char* fileName);//deletes all the negative numbers from given file
void deleteLines(char* fileName,char* fileName2,int linesToRemove);//deletes lines from first parameter and copies to the second parameter, and the number of the lines to delete is third parameter



//for the record i didnt think throughly the numbers of mutex variables at first, mutex2 is for numbers3.txt and mutex3 is for the numbers2.txt,
//just so you dont confuse when you read the code
sem_t mutex1;//for numbers1.txt -> This file is given as a resource
sem_t mutex2;//for numbers3.txt -> This file is created by the program as a third resource
sem_t mutex3;//for numbers2.txt -> This file is given as a resource
sem_t turnMutex; // for the turns between A type threads



int main()
{
	 srand(time(NULL));	//to get more accurate numbers in terms of randomness**
   
	pthread_t threadA0;
	pthread_t threadA1;
	pthread_t threadA2;
	pthread_t threadA3;
	pthread_t threadB;
	pthread_t threadC;
    
    int i[6];
    // argument for threads
    i[0] = 0; 
    i[1] = 1;
    i[2] = 2;
    i[3] = 3;
    i[4] = 4;
    i[5] = 5;
    
    //control block for semaphore initialization
	if(sem_init(&mutex1, 0, 1)==-1){
	printf("Semaphore initialization error...\n");
	exit(0);
	}

	if(sem_init(&mutex2, 0, 1)==-1){
	printf("Semaphore initialization error...\n");
	exit(0);
	}

	if(sem_init(&mutex3, 0, 1)==-1){
	printf("Semaphore initialization error...\n");
	exit(0);
	}

	if(sem_init(&turnMutex, 0, 1)==-1){
	printf("Semaphore initialization error...\n");
	exit(0);
	}


                                 
 	//control blocks for the threads creations                               

	if(pthread_create (&threadA0, NULL, (void *) &A, (void *) &i[0])!=0){
	printf("Thread A0 couldnt be created...\n");
	exit(0);
	}else currentRunningThreads++;

	if(pthread_create (&threadA1, NULL, (void *) &A, (void *) &i[1])!=0){
	printf("Thread A1 couldnt be created...\n");
	exit(0);
	}else currentRunningThreads++;

	if(pthread_create (&threadA2, NULL, (void *) &A, (void *) &i[2])!=0){
	printf("Thread A2 couldnt be created...\n");
	exit(0);
	}else currentRunningThreads++;

	if(pthread_create (&threadA3, NULL, (void *) &A, (void *) &i[3])!=0){
	printf("Thread A3 couldnt be created...\n");
	exit(0);
	}else currentRunningThreads++;

	if(pthread_create (&threadB, NULL, (void *) &B, (void *) &i[4])!=0){
	printf("Thread B couldnt be created...\n");
	exit(0);
	}else currentRunningThreads++;

	if(pthread_create (&threadC, NULL, (void *) &C, (void *) &i[5])!=0){
	printf("Thread C couldnt be created...\n");
	exit(0);
	}else currentRunningThreads++;

	//control blocks for thread joining

	if(pthread_join(threadA0, NULL)!=0){
	printf("Thread A0 join failed...\n");
	exit(0);
	}
	
	if(pthread_join(threadA1, NULL)!=0){
	printf("Thread A1 join failed...\n");
	exit(0);
	}

	if(pthread_join(threadA2, NULL)!=0){
	printf("Thread A2 join failed...\n");
	exit(0);
	}

	if(pthread_join(threadA3, NULL)!=0){
	printf("Thread A3 join failed...\n");
	exit(0);
	}

	if(pthread_join(threadB, NULL)!=0){
	printf("Thread B join failed...\n");
	exit(0);
	}

	if(pthread_join(threadC, NULL)!=0){
	printf("Thread C join failed...\n");
	exit(0);
	}

	

     // control blocks for destroy semaphore
                  


	if(sem_destroy(&mutex1)!=0){
	printf("Semaphore destroy failed (mutex1)...\n");
	exit(0);
	}
	if(sem_destroy(&mutex2)!=0){
	printf("Semaphore destroy failed (mutex2)...\n");
	exit(0);
	}
	if(sem_destroy(&mutex3)!=0){
	printf("Semaphore destroy failed (mutex3)...\n");
	exit(0);
	}
	if(sem_destroy(&turnMutex)!=0){
	printf("Semaphore destroy failed (turnmutex)...\n");
	exit(0);
	}

	printf("Program succesfully finished...\n");
        exit(0);
} // end main




//Thread A's function
void A ( void *ptr )
{	
	int x,r; 
  	x = *((int *) ptr);

	while(1){
	//checks if both resourc files are empty, if so terminates all A type threads
	if(isResource1Empty==1&&isResource2Empty==1){
		printf("%d. Thread A is terminating...\n",x+1);
		currentRunningThreads--;//decrease running thread number
		if(currentRunningThreads==2)// if all 4 A type threads are terminated 
		{
		printf("All Thread A type threads are finished...\n");
		}
		pthread_exit(&thread_exit_1);	
	}

	sem_wait(&turnMutex); //A threads works one by one in turn this would prevent starvation of Thread A type threads
	if(turn==x){
   	
 	r = rand() % 2;
	int linesToRemove= rand()%10+1;

	//this if else block will decide cut paste thread's resource file**
	// if its 0 then it will cut from numbers1 to numbers3
	// otherwise it will cut from numbers2 to numbers3.
	//Also, an A type thread will lock the numbers1.txt or numbers2.txt
	//then it will try to lock numbers3.txt to perform cut and paste operation
	//by this way A type threads will not collide with each other.
	if(r==0){
	sem_wait(&mutex1); 
	sem_wait(&mutex2);
	deleteLines(RESOURCE1,RESOURCE3,linesToRemove);
	sem_post(&mutex2); 
	sem_post(&mutex1);
	}
	else{
	sem_wait(&mutex3); 
	sem_wait(&mutex2);
	deleteLines(RESOURCE2,RESOURCE3,linesToRemove);
	sem_post(&mutex2); 
	sem_post(&mutex3);
	}

turn++;//give the turn the other A type thread

}//end if(turn==x) 
sem_post(&turnMutex);
if(turn==4)//if turn reaches max rewind it to the beginning
turn=0;
}//while end
}//end func A 

//Thread B's function
void B ( void *ptr )
{
	 int x; 
 	 x = *((int *) ptr);

	
	while(1){
	int r = rand() % 3;// this will decide which resource B will use
	
	//if all A type threads are terminated then it will clear numbers3.txt one last time to be sure it is clear
	//then it will terminate itself also
	if(currentRunningThreads==1){
	sem_wait(&mutex2); 
	deletePrimes(RESOURCE3);
	sem_post(&mutex2);
	currentRunningThreads--;
	printf("Thread B is terminating...\n");
	pthread_exit(&thread_exit_1);
}
	
	if(r==0){	//clears numbers1.txt
	sem_wait(&mutex1); 
	deletePrimes(RESOURCE1);
	sem_post(&mutex1);
}
	if(r==1){	//clears numbers3.txt
	sem_wait(&mutex2); 
	deletePrimes(RESOURCE3);
	sem_post(&mutex2);
}
	else{		//clears numbers2.txt
	sem_wait(&mutex3); 
	deletePrimes(RESOURCE2);
	sem_post(&mutex3);
}

}//while loop end

}//end void B

void C ( void *ptr )
{
	 int x; 
 	 x = *((int *) ptr);
while(1){
	int r = rand() % 3;
	if(currentRunningThreads==2){
	sem_wait(&mutex2); 
	deleteNegatives(RESOURCE3);
	sem_post(&mutex2);
	currentRunningThreads--;
	printf("Thread C is terminating...\n");
	pthread_exit(&thread_exit_1);
}

	if(r==0){	
	sem_wait(&mutex1); 
	deleteNegatives(RESOURCE1);
	sem_post(&mutex1);
}
	if(r==1){
	sem_wait(&mutex2); 
	deleteNegatives(RESOURCE3);
	sem_post(&mutex2);
}
	else{
	sem_wait(&mutex3); 
	deleteNegatives(RESOURCE2);
	sem_post(&mutex3);
}
}//while loop end

}//end void C

//function controls if the given number is prime or not
//returns 1 if prime, 0 if not.
int is_prime(int num)
{
     if (num <= 1) return 0;
     if (num % 2 == 0 && num > 2) return 0;
     for(int i = 3; i < num / 2; i+= 2)
     {
         if (num % i == 0)
             return 0;
     }
     return 1;
}
//function that deletes all the prime numbers from given filename
void deletePrimes(char* fileName){
	
	FILE* file = NULL;
	FILE* filecopy = NULL;
	char line[10];
	int var;
	int ifPrime;
	file = fopen(fileName, "rt"); 
	filecopy = fopen("copy.txt", "wt");
	//control if file couldnt be opened
	if (file == NULL) {		
		printf("Couldn't open the file...\n");
	}
	if (filecopy == NULL) {		
		printf("Couldn't open the file...\n");
	}


	while (fgets(line, 10, file) != NULL)
	{
	var=atoi(line);
	ifPrime = is_prime(var);
	if(ifPrime==0)
	 fprintf(filecopy, "%d\n", var);
	}//while end

	//control blocks for closing and renaming files
	if(fclose(file)!=0){
	printf("File closing error...\n");
	exit(0);
	}
	if(fclose(filecopy)!=0){
	printf("File closing error...\n");
	exit(0);
	}
	if(remove(fileName)!=0){
	printf("File remove error...\n");
	exit(0);
	}
	if(rename( "copy.txt", fileName )!=0){
	printf("File rename error...\n");
	exit(0);
	}

}
//function that deletes all the negative numbers from given filename
void deleteNegatives(char* fileName){

	FILE* fileneg = NULL;
	FILE* filenegcopy = NULL;
	char line[10];
	int var;
	fileneg = fopen(fileName, "rt"); 
	filenegcopy = fopen("copy1.txt", "wt");
	//control if file couldnt be opened
	if (fileneg == NULL) {		
		printf("Couldn't open the file...\n");
	}
	if (filenegcopy == NULL) {		
		printf("Couldn't open the file...\n");
	}

while (fgets(line, 10, fileneg) != NULL)
	{
	var=atoi(line);
	if(var>=0){
	 fprintf(filenegcopy, "%d\n", var);
		}
}//while end

	//control blocks for closing and renaming files
	if(fclose(fileneg)!=0){
	printf("File closing error...\n");
	exit(0);
	}
	if(fclose(filenegcopy)!=0){
	printf("File closing error...\n");
	exit(0);
	}
	if(remove(fileName)!=0){
	printf("File remove error...\n");
	exit(0);
	}
	if(rename( "copy1.txt", fileName )!=0){
	printf("File rename error...\n");
	exit(0);
	}
}
//this function will cut and paste line
void deleteLines(char* fileName,char* fileName2,int linesToRemove){
	FILE* filecp = NULL;
	FILE* filecpcopy = NULL;
	FILE* filecpwrite = NULL;
	char line[10];
	int counter=0;
	filecp = fopen(fileName, "rt"); 
	filecpcopy = fopen("copycutpaste.txt", "wt");
	filecpwrite = fopen(fileName2, "a+");
	
	//control if file couldnt be opened
	if (filecp == NULL) {		
		printf("Couldn't open the file...\n");
							}
	if (filecpcopy == NULL) {		
		printf("Couldn't open the file...\n");
							}
	if (filecpwrite == NULL) {		
		printf("Couldn't open the file...\n");
	}
//check if the related file is empty						
if(fgets(line, 10, filecp)==NULL){

	if(strcmp(fileName,RESOURCE1)==0&&isResource1Empty==0){
	isResource1Empty=1;
	printf("Resource 1 is empty...\n");

	}if(strcmp(fileName,RESOURCE2)==0&&isResource2Empty==0){
	isResource2Empty=1;
	printf("Resource 2 is empty...\n");
	}	
}
	//rewind the pointer
	rewind(filecp);
	//do he cut paste operation line by line
	while (fgets(line, 10, filecp) != NULL)
	{
	if(counter<linesToRemove){
	 fprintf(filecpwrite, "%s", line);
	}
	else{
	fprintf(filecpcopy, "%s", line);
	}
	counter++;

	}//while end

	//control blocks for closing and renaming files
	if(fclose(filecp)!=0){
	printf("File closing error...\n");
	exit(0);
	}
	if(fclose(filecpcopy)!=0){
	printf("File closing error...\n");
	exit(0);
	}
	if(fclose(filecpwrite)!=0){
	printf("File closing error...\n");
	exit(0);
	}
	if(remove(fileName)!=0){
	printf("File remove error...\n");
	exit(0);
	}
	if(rename( "copycutpaste.txt", fileName )!=0){
	printf("File rename error...\n");
	exit(0);
	}
}

