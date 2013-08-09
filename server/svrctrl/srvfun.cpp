#include "svrctrl.h"
#include "conn.h"
#include "svrdll.h"
#include "macro.h"

#include <time.h>
#include <Windows.h>

class HelpClass
{
public:
	DECLARE_DLLINFO_MAP()
};

BEGIN_DLLINFO_MAP(HelpClass)
	ADD_DLLINFO_ENTRY(CONTROL_SHUTDOWN_SYSTEM, 0, "ShutdownMachine")
	ADD_DLLINFO_ENTRY(CONTROL_RESTART_SYSTEM, 0, "RebootMachine")
	ADD_DLLINFO_ENTRY(CONTROL_QUERY_INFO, 0, "QueryInfomation")

END_DLLINFO_MAP()

int GetDllFuncCnt(){return HelpClass::GetInfoCnt();}

const dll_info* GetDllFuncs(){return HelpClass::GetInfos();}


//调整当前进程特权
static BOOL EnablePrivilege(LPCTSTR lpPrivilegeName, BOOL bEnable)
{
	HANDLE hToken;
	BOOL bResult;
	TOKEN_PRIVILEGES tp;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

	bResult = LookupPrivilegeValue(NULL, lpPrivilegeName, &tp.Privileges[0].Luid);
	if (bResult == FALSE)
		return false;

	bResult = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	if (bResult == FALSE)
		return false;

	bResult = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	CloseHandle(hToken);

	return bResult;
}

#ifdef SVRCTRL_EXPORTS
#define SVRCTRL_API extern "C" __declspec(dllexport)
#else
#define SVRCTRL_API __declspec(dllimport)
#endif

SVRCTRL_API int ShutdownMachine(conn s, char* buf, int n, writeToClient fn)
{
	EnablePrivilege(SE_SHUTDOWN_NAME, TRUE);				//启用关机特权
	ExitWindowsEx(EWX_FORCE | EWX_SHUTDOWN, 0);				//关闭 Windows

	return 0;
}

SVRCTRL_API int RebootMachine(conn s, char* buf, int n, writeToClient fn)
{
	EnablePrivilege(SE_SHUTDOWN_NAME, TRUE);				//启用关机特权
	ExitWindowsEx(EWX_FORCE | EWX_REBOOT, 0);				//重启 Windows

	return 0;
}

SVRCTRL_API int QueryInfomation(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_QUERY_INFO);
	int nStatus = 0;
	system_info info;

	memset(&info, 0, sizeof(info));

	//操作系统版本
	OSVERSIONINFOEXA os; 
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA); 
	if(GetVersionExA((OSVERSIONINFOA *)&os)){ 
		info.majorVersion = os.dwMajorVersion;
		info.minorVersion = os.dwMinorVersion;
		info.buildNumber = os.dwBuildNumber;
		info.platformId = os.dwPlatformId;
		info.productType = os.wProductType;
		printf("%d %d %d %d %d\n", info.majorVersion, info.minorVersion, info.buildNumber,
			info.platformId, info.productType);
	}

	//CPU
	SYSTEM_INFO	sysInfo = {0};
	HKEY hKey;
	unsigned char tmp[280];
	ULONG cb;
	GetSystemInfo(&sysInfo);
	info.cpuCount = sysInfo.dwNumberOfProcessors;

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		cb = sizeof(tmp);
		if(RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL, tmp, &cb) == ERROR_SUCCESS)	//获取处理器名称
			strncpy(info.cpuDesc, (char*)tmp, sizeof(info.cpuDesc)-1);
		
		cb = sizeof(info.cpuSpeed);
		RegQueryValueExA(hKey, "~MHz", NULL, NULL, (LPBYTE)&info.cpuSpeed, &cb);			//获取处理器主频

		RegCloseKey(hKey);
	}

	//内存
	MEMORYSTATUS Buffer = {0};
	GlobalMemoryStatus(&Buffer);
	info.totalMemory = Buffer.dwTotalPhys/1024;
	info.availMemory = Buffer.dwAvailPhys/1024;

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS",
		0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		cb = sizeof(tmp);
		if(RegQueryValueExA(hKey, "BIOSVendor", NULL, NULL, tmp, &cb) == ERROR_SUCCESS)
			strncpy(info.biosDesc, (char*)tmp, sizeof(info.biosDesc)-1);

		cb = sizeof(tmp);
		if(RegQueryValueExA(hKey, "BIOSVersion", NULL, NULL, tmp, &cb) == ERROR_SUCCESS)
			strncat(info.biosDesc, (char*)tmp, sizeof(info.biosDesc)-1);
	}

	//计算机名称
	DWORD len = 64;
	GetComputerNameA(info.computerName, &len);

	//用户名
	len = 64;
	GetUserNameA(info.userName, &len);

	//系统目录
	char buffer[64];
	if(GetSystemDirectoryA(buffer, sizeof(buffer)))
		strncpy(info.sysDir, buffer, sizeof(info.sysDir)-1);
	if(GetWindowsDirectoryA(buffer, sizeof(buffer)))
		strncpy(info.winDir, buffer, sizeof(info.winDir)-1);

	const int maxNW = 16;
	struct network_info nws[maxNW];
	char sendBuf[maxNW*sizeof(network_info)+sizeof(info)];
	int cntNW = GetNetworkInfo(nws, maxNW);
	info.cntNW = cntNW;

	if(fn)
	{
		header.dataLen = sizeof(info) + cntNW * sizeof(struct network_info);
		memcpy(sendBuf, &info, sizeof(info));
		memcpy(sendBuf+sizeof(info), &nws[0], cntNW * sizeof(network_info));
		fn(s, &header, sendBuf, header.dataLen);
	}

	return 0;
}