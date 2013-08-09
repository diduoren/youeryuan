#ifndef SVRCTRL2_H
#define SVRCTRL2_H

#include <vector>
#include <string.h>

/** 
 根据本地网卡信息生成相应的GUID
*/
int MakeGuid(unsigned char guidV[]);

/** 
 根据主机名获得其IP地址信息
*/
int GetSiteIP(std::vector<std::string>& ips, const char* hostname);


int RealService();

/**
 调整当前进程特权
*/
bool EnablePrivilege(const char* privilege, bool enable);

#endif