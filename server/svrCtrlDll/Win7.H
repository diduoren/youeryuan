#include <WtsApi32.h>
#include <UserEnv.h>
#include "macro.h"
#pragma comment(lib,"WtsApi32.lib")
#pragma comment(lib,"UserEnv.lib")

DWORD _stdcall LaunchAppIntoDifferentSession( LPTSTR lpCommand )
{
	DWORD dwRet = 0;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	DWORD dwSessionId;
	HANDLE hUserToken = NULL;
	HANDLE hUserTokenDup = NULL;
	HANDLE hPToken = NULL;
	HANDLE hProcess = NULL;
	DWORD dwCreationFlags;

	HMODULE hInstKernel32    = NULL;
	typedef DWORD (WINAPI *WTSGetActiveConsoleSessionIdPROC)();	
	WTSGetActiveConsoleSessionIdPROC WTSGetActiveConsoleSessionId = NULL;

	hInstKernel32 = LoadLibrary("Kernel32.dll");
	if (!hInstKernel32)		
		return FALSE;	

	WTSGetActiveConsoleSessionId = (WTSGetActiveConsoleSessionIdPROC)GetProcAddress(hInstKernel32,"WTSGetActiveConsoleSessionId");
	dwSessionId = WTSGetActiveConsoleSessionId();

	do
	{
		WTSQueryUserToken( dwSessionId,&hUserToken );
		dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
		ZeroMemory( &si, sizeof( STARTUPINFO ) );
		si.cb= sizeof( STARTUPINFO );
		si.lpDesktop = "winsta0\\default";
		ZeroMemory( &pi, sizeof(pi) );
		TOKEN_PRIVILEGES tp;
		LUID luid;

		if( !::OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
			| TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID
			| TOKEN_READ | TOKEN_WRITE, &hPToken ) )
		{
			dwRet = GetLastError();
			break;
		}
		else;

		if ( !LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &luid ) )
		{
			dwRet = GetLastError();
			break;
		}
		else;
		tp.PrivilegeCount =1;
		tp.Privileges[0].Luid =luid;
		tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;

		if( !DuplicateTokenEx( hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hUserTokenDup ) )
		{
			dwRet = GetLastError();
			break;
		}
		else;

		//Adjust Token privilege
		if( !SetTokenInformation( hUserTokenDup,TokenSessionId,(void*)&dwSessionId,sizeof(DWORD) ) )
		{
			dwRet = GetLastError();
			break;
		}
		else;

		if( !AdjustTokenPrivileges( hUserTokenDup, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, NULL ) )
		{
			dwRet = GetLastError();
			break;
		}
		else;

		LPVOID pEnv =NULL;
		if( CreateEnvironmentBlock( &pEnv, hUserTokenDup, TRUE ) )
		{
			dwCreationFlags|=CREATE_UNICODE_ENVIRONMENT;
		}
		else pEnv=NULL;

		// Launch the process in the client's logon session.
		if( CreateProcessAsUser(    hUserTokenDup,    // client's access token
			NULL,        // file to execute
			lpCommand,        // command line
			NULL,            // pointer to process SECURITY_ATTRIBUTES
			NULL,            // pointer to thread SECURITY_ATTRIBUTES
			FALSE,            // handles are not inheritable
			dwCreationFlags,// creation flags
			pEnv,          // pointer to new environment block
			NULL,          // name of current directory
			&si,            // pointer to STARTUPINFO structure
			&pi            // receives information about new process
			) )
		{
		}
		else
		{
			dwRet = GetLastError();
			break;
		}
	}
	while( 0 );

	KDebug("dwRet = %d;", dwRet);

	//Perform All the Close Handles task
	if( NULL != hUserToken )
	{
		CloseHandle( hUserToken );
	}
	else;

	if( NULL != hUserTokenDup)
	{
		CloseHandle( hUserTokenDup );
	}
	else;

	if( NULL != hPToken )
	{
		CloseHandle( hPToken );
	}
	else;

	return dwRet;
}
