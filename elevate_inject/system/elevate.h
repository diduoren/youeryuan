#pragma once

#include <windows.h>

bool OpenProcessToInject(HANDLE *pOutProcHandle, DWORD dwPid, const wchar_t *szProcName);

DWORD MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName);

template < typename T > class GetProcAddr
{
public:
	T f;

	__forceinline GetProcAddr(FARPROC (WINAPI *fpGetProcAddress)(HMODULE hModule, LPCSTR lpProcName), HMODULE hModule, const char *lpProcName)
	{
		f = reinterpret_cast< T >(fpGetProcAddress(hModule, lpProcName));
	}
};


class CRemoteMemory
{
private:
	HANDLE m_hRemoteProcess;
	bool m_bAnyFailures;

private:
	CRemoteMemory(const CRemoteMemory &rhs); // Disallow.
	CRemoteMemory &operator=(const CRemoteMemory &rhs); // Disallow.

public:
	CRemoteMemory(HANDLE hRemoteProcess);
	virtual ~CRemoteMemory();
	void LeakMemory();
	bool AnyFailures() const;
	void *AllocAndCopyMemory(const void *pLocalBuffer, SIZE_T bufferSize, bool bExecutable, bool bConst = true);
	wchar_t *AllocAndCopyMemory(const wchar_t *szLocalString, bool bConst = true);
	char *AllocAndCopyMemory(const char *szLocalString, bool bConst = true);
};