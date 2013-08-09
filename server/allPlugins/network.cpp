#include <winsock2.h>
#include <iphlpapi.h>
#include <Nb30.h>
#include "svrctrl.h"
#include "macro.h"

using namespace std;

typedef struct _ASTAT_{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[30];
}ASTAT,*PASTAT;


int MakeGuid(unsigned char guidV[])
{	
	GUID guid;
	char buf1[256] = "\0", buf3[256] = "\0";
	unsigned long buf2;
	char* lpVolName = buf1;
	unsigned long* lpVolSN = &buf2;
	unsigned long dwSysFlags;         
	char FileSysNameBuf[256];

	HMODULE hModule = LoadLibraryA("NetAPI32.dll");
	if(hModule == NULL)	return -1;


	typedef UCHAR (__stdcall *fNetBios)(PNCB);
	fNetBios f = (fNetBios)GetProcAddress(hModule, "Netbios");
	if(f == NULL)	return -1;

	memset(&guid , 0 , sizeof(guid));

	if(GetVolumeInformationA( "C:\\", lpVolName, 256, lpVolSN, NULL, &dwSysFlags, FileSysNameBuf, 256) == FALSE)
		return -1;

	NCB Ncb;
	LANA_ENUM lenum;
	
	//向网卡发送NCBENUM命令，以获取当前机器的网卡信息，如有多少个网卡、每张网卡的编号等
	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);
	if (f(&Ncb) != 0 || lenum.length < 1) 
		return -1;	

	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBRESET;
	Ncb.ncb_lana_num = lenum.lana[0];//第一个网卡
	if(f(&Ncb) != 0)
		return -1;	

	//NCBASTAT命令以获取网卡的信息
	ASTAT Adapter;
	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBASTAT;
	Ncb.ncb_lana_num = lenum.lana[0];
	strcpy((char*)Ncb.ncb_callname,  "*   ");
	Ncb.ncb_buffer = (unsigned char *)&Adapter;
	Ncb.ncb_length = sizeof(Adapter);

	if(f(&Ncb) != 0)	return -1;
	
	//合并成GUID格式
	guid.Data1 = *lpVolSN;
	guid.Data2 = unsigned short((*lpVolSN) & 0x0000FFFF);
	guid.Data3 = unsigned short(((*lpVolSN) >> 16) & 0x0000FFFF);
	memcpy(guid.Data4 , Adapter.adapt.adapter_address, 6);
	guid.Data4[6] = unsigned char((*lpVolSN) & 0x000000FF);
	guid.Data4[7] = unsigned char(((*lpVolSN) >> 16) & 0x000000FF);	
	memcpy(guidV, &guid, sizeof(guid));

	return 0;
}

int GetSiteIP(vector<string>& ips, const char* hostname)
{
	HMODULE module = LoadLibraryA("ws2_32.dll");
	if(module == NULL)	return -1;

	typedef struct hostent* (__stdcall *myGetHostByName)(const char*);
	typedef char* (__stdcall* myInet_ntoa)(struct in_addr);
	
	myGetHostByName myGetHost = (myGetHostByName)GetProcAddress(module, "gethostbyname");
	myInet_ntoa myInet = (myInet_ntoa)(myGetHostByName)GetProcAddress(module, "inet_ntoa");
	if(!myGetHost || !myInet)
	{
		FreeLibrary(module);
		return -1;
	}

	hostent *pHostent = myGetHost(hostname);
	if(pHostent == NULL)	return -1;

	ips.clear();
	hostent& he = *pHostent;
	sockaddr_in sa;	
	for (int i = 0; he.h_addr_list[i]; i++)
	{
		memcpy(&sa.sin_addr.s_addr, he.h_addr_list[i], he.h_length);
		char* ip = myInet(sa.sin_addr);
		//printf("address:%s\n", ip);
		ips.push_back(ip);
	}

	FreeLibrary(module);
	return 0;
}

bool EnablePrivilege(const char* privilege, bool enable)
{
	HANDLE hToken;
	bool   bResult = true;
	TOKEN_PRIVILEGES tp;
	
	HMODULE hModule = GetModuleHandleA("advapi32.dll");
	if(hModule == NULL || (hModule = LoadLibraryA("advapi32.dll")) == NULL)
		return false;

	FARPROC fLookupPrivilege = GetProcAddress(hModule, "LookupPrivilegeValueA");
	FARPROC fOpenProcToken = GetProcAddress(hModule, "OpenProcessToken");
	FARPROC fAdjustToken = GetProcAddress(hModule, "AdjustTokenPrivileges");

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

	bResult = ((BOOL (__stdcall*)(LPCSTR, LPCSTR, PLUID))fLookupPrivilege) //LookupPrivilegeValue
					(NULL, privilege, &tp.Privileges[0].Luid);
	if (bResult == FALSE)
		return false;

	bResult = ((BOOL (__stdcall*)(HANDLE, DWORD, PHANDLE))fOpenProcToken)//OpenProcessToken
				(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	if (bResult == FALSE)
		return false;

	bResult = ((BOOL (__stdcall*)(HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD))fAdjustToken)
					//AdjustTokenPrivileges
					(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	CloseHandle(hToken);

	return bResult;
}