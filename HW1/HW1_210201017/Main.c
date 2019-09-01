#include <stdio.h>
#include <windows.h>

#include <tchar.h>
#include <strsafe.h>


#define NO_OF_PROCESS 7

#define BUFSIZE 4096 


#pragma warning(disable : 4996)  


int main(int argc, char* argv[])
{
	STARTUPINFO si[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	SECURITY_ATTRIBUTES sa[NO_OF_PROCESS];

	HANDLE writePipe[NO_OF_PROCESS], readPipe[NO_OF_PROCESS];
	HANDLE processHandles[NO_OF_PROCESS];

	char message[1];
	int bytesToWrite = 0;
	int bytesWritten = 0;

	
	HANDLE g_hInputFile = NULL;


	HANDLE Child_Out_ReadPipe[NO_OF_PROCESS],Child_Out_WritePipe[NO_OF_PROCESS];

	//creating security attributes
	int a = 0;
	for (a = 0; a < NO_OF_PROCESS; a++)
	{
		SecureZeroMemory(&sa[a], sizeof(SECURITY_ATTRIBUTES));
		sa[a].bInheritHandle = TRUE;
		sa[a].lpSecurityDescriptor = NULL;
		sa[a].nLength = sizeof(SECURITY_ATTRIBUTES);
	}



	//creating the pipe
	int e = 0;
	for (e = 0; e < NO_OF_PROCESS; e++)
	{
		// Create a pipe for the child process's STDOUT. 

		if (!CreatePipe(&Child_Out_ReadPipe[e], &Child_Out_WritePipe[e], &sa[e], 0)) 
		{
			printf("unable to create pipe\n");
			system("pause");
			exit(0);
		}

		// Ensure the read handle to the pipe for STDOUT is not inherited.

		if (!SetHandleInformation(Child_Out_ReadPipe[e], HANDLE_FLAG_INHERIT, 0))
		{
			printf("unable to set handle information_Out\n");
			system("pause");
			exit(0);
		}




		// Create a pipe for the child process's STDIN. 
		if (!CreatePipe(&readPipe[e], &writePipe[e], &sa[e], 0)) //use the default buffer size.
		{
			printf("unable to create pipe\n");
			system("pause");
			exit(0);
		}

		// Ensure the write handle to the pipe for STDIN is not inherited. 
		if (!SetHandleInformation(writePipe[e], HANDLE_FLAG_INHERIT, 0))
			printf("Error SetHandleInformation_In");



	}
	


	
	//creating variables for child process
	int b = 0;
	for (b = 0; b < NO_OF_PROCESS;b++)
	{
		SecureZeroMemory(&si[b], sizeof(STARTUPINFO));
		SecureZeroMemory(&pi[b], sizeof(PROCESS_INFORMATION));

		si[b].cb = sizeof(STARTUPINFO);
		si[b].hStdInput = readPipe[b];
		si[b].hStdOutput = Child_Out_WritePipe[b];
	
		si[b].hStdError = Child_Out_WritePipe[b]; 
		si[b].dwFlags = STARTF_USESTDHANDLES;
	}

	



	//create child process

	char* lpCommandLine[NO_OF_PROCESS] = { "Child.exe 1", "Child.exe 2", "Child.exe 3","Child.exe 4"};

	int i = 0;

	for (i = 0; i < NO_OF_PROCESS; i++)
	{

		if (!CreateProcess(NULL,
			lpCommandLine[i],
			NULL,
			NULL,
			TRUE,
			CREATE_NEW_CONSOLE,
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
		}

	}

	int c = 0;
	for (c = 0; c <NO_OF_PROCESS;c++)
	{
		
				
		message[0] = c+'1';
		bytesToWrite = strlen(message);
		bytesToWrite++;
		if (!WriteFile(writePipe[c], message, bytesToWrite, &bytesWritten, NULL))
			{
				printf("unable to write to pipe\n");
				system("pause");
				exit(0);
			}
	}
	for (c = 0; c < NO_OF_PROCESS; c++)
	{
		if (!CloseHandle(writePipe[c])) {
			system("pause");
			exit(0);
		}
	}


	printf("number of bytes: %d  --  number of bytes sent: %d\n", bytesToWrite, bytesWritten);
	WaitForMultipleObjects(NO_OF_PROCESS, processHandles, TRUE, INFINITE);

	//-----------------------------------------------------------------------------------------------------------------
			DWORD dwRead1;
			CHAR chBuf1[BUFSIZE];
			  BOOL bSuccess = FALSE;
			

			int u = 0;
			for ( u = 0; u < NO_OF_PROCESS; u++)
			{	
					bSuccess = ReadFile(Child_Out_ReadPipe[u], chBuf1, BUFSIZE, &dwRead1, NULL);
					if (!bSuccess || dwRead1 == 0) break;
				
					printf("Message : %s",chBuf1);

					
			}

	int d = 0;
	for (d = 0; d < NO_OF_PROCESS; d++)
	{
		CloseHandle(pi[d].hThread);
		CloseHandle(pi[d].hProcess);	
	}
	system("pause");
	return 1;

}



