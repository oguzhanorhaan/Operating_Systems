#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>


#pragma warning(disable : 4996)  

#define BUFSIZE 4096 


//thread structure definition
typedef struct
{
    char* day;
	int* NUM_OF_MILK;
	int* NUM_OF_CHIPS;
	int* NUM_OF_BISCUIT;
	int* NUM_OF_COKE;
	int* sum;
	int* threadNo;
}THREAD_PARAMETERS;

DWORD WINAPI threadWork1(LPVOID parameters);

int AtoiMessageDay = 0;

#define threadNumber 4


int milk = 0;
int chips = 0;
int biscuit = 0;
int coke = 0;

int main(int argc, char* argv[])
{
	// parameter definition region
	char message[1];
	int i = 0;

	HANDLE* handles;
	CHAR chBuf[BUFSIZE];

	DWORD dwRead, dwWritten, bytesToWrite;
	HANDLE* hStdin, hStdout;
	BOOL bSuccess;


	THREAD_PARAMETERS* lpParameter;
	int* threadID;
	int threadCount = 0;


	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (
		(hStdout == INVALID_HANDLE_VALUE) ||
		(hStdin == INVALID_HANDLE_VALUE)
		)
		ExitProcess(1);



	//check arguments for error
	if (argc != 2)
	{
		printf("error in child process...now exiting %d\n", argv[0]);
		system("pause");
		exit(0);
	}

	//read thread count from arguments
	threadCount = atoi(argv[1]); //process id

								 //allocate memory for every parameters needed region
	handles = malloc(sizeof(HANDLE)* threadNumber);
	lpParameter = malloc(sizeof(THREAD_PARAMETERS)* threadNumber);
	threadID = malloc(sizeof(int)* threadNumber);

		// Read from standard input and stop on error or no data.
		bSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);
		
		if (!bSuccess || dwRead == 0)
		{

		}


		int v = atoi(chBuf);

		AtoiMessageDay = v;


	//for each thread
	for (i = 0; i < threadNumber; i++)
	{
		//initialize parameters
		lpParameter[i].day = AtoiMessageDay;
		lpParameter[i].threadNo = i + 1;

	
			handles[i] = CreateThread(NULL, 0, threadWork1, &lpParameter[i], 0, &threadID[i]);
				
		printf("thread %d has started working with id: %d\n", i + 1, threadID[i]);

	}

	
	WaitForMultipleObjects(threadNumber, handles, TRUE, INFINITE);


	char dayAndResults[100];
	

	//results
	for ( i = 0; i < threadNumber; i++)
	{
	
		milk=milk+(int)lpParameter[i].NUM_OF_MILK;
		chips =chips+ (int)lpParameter[i].NUM_OF_CHIPS;
		biscuit= biscuit+(int)lpParameter[i].NUM_OF_BISCUIT;
		coke = coke+(int)lpParameter[i].NUM_OF_COKE;
    
	}
	
	sprintf_s(dayAndResults,sizeof(dayAndResults),"%d%d%d%d%d",AtoiMessageDay,milk,chips,biscuit,coke);

	bSuccess = WriteFile(hStdout, dayAndResults, dwRead, &dwWritten, NULL);

	if (!bSuccess)
	{
		printf("Error occured");
	}

	for (i = 0; i < threadNumber; i++)
	{
	
		CloseHandle(handles[i]);
	}

	free(handles);
	free(lpParameter);
	free(threadID);

	system("pause");
	return 1;
}


DWORD WINAPI threadWork1(LPVOID parameters)
{
	THREAD_PARAMETERS* param = (THREAD_PARAMETERS*)parameters;
	Sleep(1000);

	//File operation



	FILE *fp;
	long lSize;
	char *buffer;

	fp = fopen("market.txt", "rb");
	if (!fp) perror("market.txt"), exit(1);

	fseek(fp, 0L, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	///* allocate memory for entire content */
	buffer = calloc(1, lSize + 1);
	if (!buffer) fclose(fp), fputs("memory alloc fails", stderr), exit(1);
	//
	///* copy the file into the buffer */
	if (1 != fread(buffer, lSize, 1, fp))
		fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

	char *s = buffer;

	char x[50], y[50];

	sprintf_s(x, sizeof(x), "%s%d%s", "START DAY ", AtoiMessageDay, "#");

	sprintf_s(y, sizeof(y), "%s%d%s", "END DAY ", AtoiMessageDay, "#");

	char *PATTERN1 = x;
	char *PATTERN2 = y;

	char *target = NULL;
	char *start, *end;

	if (start = strstr(s, PATTERN1))
	{
		start += strlen(PATTERN1);
		if (end = strstr(start, PATTERN2))
		{
			target = (char *)malloc(end - start + 1);
			memcpy(target, start, end - start);
			target[end - start] = '\0';
		}
	}

	//// end of file region
	if (param->threadNo == 1)
	{
		int count = 0;
		const char *tmp = target;
		while (tmp = strstr(tmp, "MILK"))
		{
			count++;
			tmp++;
		}
		param->NUM_OF_MILK = count;
		milk = count;
		//printf("OCCURENCE OF THE MILK IS %d\n", param->NUM_OF_MILK);

	}
	else if (param->threadNo == 2)
	{
		int count = 0;
		const char *tmp = target;
		while (tmp = strstr(tmp, "COKE"))
		{
			count++;
			tmp++;
		}
		param->NUM_OF_COKE = count;
		coke = count;
		//printf("OCCURENCE OF THE COKE IS %d\n", param->NUM_OF_COKE);

	}
	else if (param->threadNo == 3)
	{
		int count = 0;
		const char *tmp = target;
		while (tmp = strstr(tmp, "BISCUIT"))
		{
			count++;
			tmp++;
		}
		param->NUM_OF_BISCUIT = count;
		biscuit = count;
		//printf("OCCURENCE OF THE BISCUIT IS %d\n", param->NUM_OF_BISCUIT);

	}
	else if (param->threadNo == 4)
	{
		int count = 0;
		const char *tmp = target;
		while (tmp = strstr(tmp, "CHIPS"))
		{
			count++;
			tmp++;
		}
		param->NUM_OF_CHIPS = count;
		chips = count;
		//printf("OCCURENCE OF THE CHIPS IS %d\n", param->NUM_OF_CHIPS);

	}


	fclose(fp);
	free(target);
	free(buffer);

	system("pause");
	return 1;
}





