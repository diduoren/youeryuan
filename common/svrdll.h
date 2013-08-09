#ifndef SVRDLL_H
#define SVRDLL_H

typedef struct dll_info
{
	unsigned short cmd;
	unsigned short reserved;
	char name[32]; 
}dll_info;

typedef void* conn;

typedef const dll_info* (*getDllInfo)();
typedef const int (*getFuncCnt)();
typedef int (*writeToClient)(conn s, struct control_header* header, const char* buf, int n);


#define DECLARE_DLLINFO_MAP()					\
	static int GetInfoCnt();					\
	static const dll_info* GetInfos();		\
	static const dll_info allInfos[];		

#define BEGIN_DLLINFO_MAP(theClass)				\
	int theClass::GetInfoCnt(){int count = 0; while(allInfos[count].cmd) count++; return count;}	\
	const dll_info* theClass::GetInfos() {	return allInfos;	}		\
	const dll_info theClass::allInfos[] = {											

#define ADD_DLLINFO_ENTRY(id, reserved, name)	\
	{id, reserved, name},

#define END_DLLINFO_MAP()			\
		{0, 0, ""}		};	


#endif