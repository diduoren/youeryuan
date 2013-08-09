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

int GetSiteIP(string_ip ips[], int* nips, const char* hostname)
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

	hostent& he = *pHostent;
	sockaddr_in sa;	
	*nips = 0;
	for (int i = 0; he.h_addr_list[i]; i++)
	{
		if(*nips >= 32)	break;
		memcpy(&sa.sin_addr.s_addr, he.h_addr_list[i], he.h_length);
		strcpy_s(ips[*nips], 32, myInet(sa.sin_addr));
		*nips ++;
	}

	FreeLibrary(module);
	return 0;
}

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