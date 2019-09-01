#include<stdio.h>
#include <windows.h>

#define NO_OF_PROCESS 5
#define BUFFER_SIZE 1024
#pragma warning(disable : 4996)  

struct Process_Information  //structure that contain the information about process
{
	int id; //id of process
	double bursttime; //burst time of processes
};

double NextBurstTime(double actuallength, double bursttime) //function to calculate the next burst time
{
	double ALPHA = 0.5;
	double result = (ALPHA*actuallength + (1 - ALPHA)*bursttime);  //Apply the formula and calculate the next brust time
	return result;//return the result
}

void Sort_SJF(struct Process_Information array[]) //function that sort the array according to Shortest job first
{
	/**Bubble Sort to sort the array according to Shortest Jo first**/
	int i = 0, j = 0;  //declare variables for loops
	struct Process_Information  swap; //temporary instance that swap values
	for (i = 0; i < NO_OF_PROCESS; i++)  //outer loop
	{
		for (j = 0; j < NO_OF_PROCESS - i - 1; j++) //inner loop
		{
			if (array[j].bursttime > array[j + 1].bursttime) /* For decreasing order use < */
			{
				swap = array[j]; //swap the objects
				array[j] = array[j + 1];
				array[j + 1] = swap;
			}
		}
	}
}

void orderexecution(struct Process_Information array[])
{
	/** tell the execution of order**/
	int i = 0;
	printf("Execution Order --->   <");
	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		printf("P%d ", array[i].id);
	}
	printf(">");
	printf("\n");
}

void print(struct Process_Information array[], int RandomValues[])//print the variables
{
	int i;

	printf("Process \t Burst Time \t Actual length \t BurstTime+1 \n");
	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		int randomnumber = RandomValues[i]; //get the random variable between 50 and 300
		double nextbursttime = NextBurstTime(randomnumber, array[i].bursttime); //apply the formula and get the next brust time
		printf("\n P%d \t\t %.1lf \t\t %d \t\t %.1lf \n", array[i].id, array[i].bursttime, randomnumber, nextbursttime);
		array[i].bursttime = nextbursttime; //set the value of burst time to new value
	}
	printf("\n\n");
}





int main(int argc, char* argv[])
{

	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];

	HANDLE namedPipeHandle[NO_OF_PROCESS];

	struct Process_Information process[] = { { 1,300 },{ 2,220 },{ 3,180 },{ 4,45 },{ 5,255 } };

	char* x[NO_OF_PROCESS] = { "\\\\.\\Pipe\\MyNamedPipe1", "\\\\.\\Pipe\\MyNamedPipe2", "\\\\.\\Pipe\\MyNamedPipe3", "\\\\.\\Pipe\\MyNamedPipe4",  "\\\\.\\Pipe\\MyNamedPipe5" };

	char* lpCommandLine[NO_OF_PROCESS] = { "Child.exe 1", "Child.exe 2", "Child.exe 3", "Child.exe 4", "Child.exe 5" };

	int a;
	for (a = 0; a < NO_OF_PROCESS; a++)
	{

		SecureZeroMemory(&si[a], sizeof(STARTUPINFO));
		si[a].cb = sizeof(STARTUPINFO);
		SecureZeroMemory(&pi[a], sizeof(PROCESS_INFORMATION));


		namedPipeHandle[a] = CreateNamedPipe(
			x[a],             // pipe name 
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE |
			PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUFFER_SIZE,
			BUFFER_SIZE,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);

		if (INVALID_HANDLE_VALUE == namedPipeHandle[a])
		{
			printf("\nError occurred while creating the pipe: %d", GetLastError());
			return 1;
		}

		if (!CreateProcess(NULL,
			lpCommandLine[a],
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&si[a],
			&pi[a]))
		{
			printf("Unable to create process: %d\n", a);
			system("pause");
			ExitProcess(0);
		}
		else
		{
			processHandles[a] = pi[a].hProcess;
			printf("parent has created process number %d\n", a+1);
		}


		BOOL IsChildConnected = ConnectNamedPipe(namedPipeHandle[a], NULL);

		if (FALSE == IsChildConnected)
		{
			printf("\nError occurred while connecting to child: %d", GetLastError());
			CloseHandle(namedPipeHandle[a]);
			return 1;
		}



	}

	char myBuffer[BUFFER_SIZE] = "selam child ben parent\n";
	DWORD cbBytes;

	for (size_t i = 0; i < 5; i++)
	{
		int MessageRandomValue[NO_OF_PROCESS];  //SJF yaptýktan sonra sýrasýyla processler random value oluþturuyor. Bu array o sýraya göre random value tutuyor

		Sort_SJF(process); //SJF 
		orderexecution(process); 	/** tell the execution of order**/

		for (a = 0; a < NO_OF_PROCESS; a++) {
			BOOL IsOkey = WriteFile(
				namedPipeHandle[(process[a].id) - 1],   // SJF Sonucunda çýkan sýralamaya göre process indexini veriyoruz
				myBuffer,
				strlen(myBuffer) + 1,
				&cbBytes,
				NULL);

			if ((!IsOkey) || (strlen(myBuffer) + 1 != cbBytes))
			{
				printf("\nError occurred while writing to child: %d", GetLastError());
				CloseHandle(namedPipeHandle[(process[a].id) - 1]);
				return 1;
			}

			//Read child's message
			IsOkey = ReadFile(
				namedPipeHandle[(process[a].id) - 1], // SJF Sonucunda çýkan sýralamaya göre process indexini veriyoruz              
				myBuffer,
				sizeof(myBuffer),
				&cbBytes,
				NULL);


			if ((!IsOkey) || (0 == cbBytes))
			{
				printf("\nError occurred while reading from the child: %d", GetLastError());
				CloseHandle(namedPipeHandle[(process[a].id) - 1]);
				return 1;
			}

			printf("\n %d Child sent the message: %s\n", (process[a].id), myBuffer);



			MessageRandomValue[a] = atof(myBuffer); // process array i SJF ye göre sýralandýðýndan gelen mesajlarda sýralanmýþ oluyor ve onlarý bu arrayde saklýyoruz
		}

		print(process, MessageRandomValue); // yukarýda tanýmlanan print methodunu çaðýrýr.  SJF ye göre sýralnmýþ arrayi ve onlarýn childdan alýnan random length lerini parametre olarak veriyoruz.

	}


	//Close Handles
	for (size_t b = 0; b < NO_OF_PROCESS; b++)
	{
		CloseHandle(pi[b].hThread);
		CloseHandle(pi[b].hProcess);
		CloseHandle(namedPipeHandle[b]);
	}

	system("pause");
	return 0;
}