#include<stdio.h>
#include <windows.h>
#include <time.h>

#define BUFFER_SIZE 1024 
#pragma warning(disable : 4996) 
#define NO_OF_PROCESS 5

double generaterandom() { //function that generate the random variable
	return (50 + rand() % 300); //generate the random variables between 50 and 300
}


int main(int argc, char* argv[])
{

	HANDLE namedPipeHandle;
	char* x[NO_OF_PROCESS] = { "\\\\.\\Pipe\\MyNamedPipe1", "\\\\.\\Pipe\\MyNamedPipe2", "\\\\.\\Pipe\\MyNamedPipe3", "\\\\.\\Pipe\\MyNamedPipe4",  "\\\\.\\Pipe\\MyNamedPipe5" };



	if (argc != 2)
	{
		printf("error in child process. %d\n", argv[0]);
		system("pause");
		exit(0);
	}
	int pid = atoi(argv[1]); //Proces id

	time_t t;
	srand((int)time(&t) % getpid());  // to generate different numbers at every child and every iteration

//Connect to the Parent
	namedPipeHandle = CreateFile(
		x[pid - 1],   // pipe name 
		GENERIC_READ |
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == namedPipeHandle)
	{
		printf("\nError occurred while connecting to Parent: %d", GetLastError());
		return 1;
	}

	char myBuffer[BUFFER_SIZE];
	DWORD cbBytes;


	for (size_t i = 0; i < 5; i++)
	{
		double randm = generaterandom();  //random number generate

		//read message from parent process
		BOOL IsOkey = ReadFile(
			namedPipeHandle,
			myBuffer,
			sizeof(myBuffer),
			&cbBytes,
			NULL);
		if ((!IsOkey) || (0 == cbBytes))
		{
			printf("\nError occurred while reading  from Parent: %d", GetLastError());
			CloseHandle(namedPipeHandle);
			return 1;
		}

		Sleep(randm);  // Works as actual length


		//sends message to the parent 
		char message[20];
		sprintf(message, "%f", randm);

		IsOkey = WriteFile(
			namedPipeHandle,
			message,
			strlen(message) + 1,
			&cbBytes,
			NULL);

		if ((!IsOkey) || (strlen(message) + 1 != cbBytes))
		{
			printf("\nError occurred while writing to Parent: %d", GetLastError());
			CloseHandle(namedPipeHandle);
			return 1;
		}
		else
		{
			printf("Message sent to parent --> %s \n",message);
		}

	}


	CloseHandle(namedPipeHandle);

	system("pause");
	return 0;
}