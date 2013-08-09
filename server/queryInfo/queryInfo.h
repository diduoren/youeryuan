#ifndef QUERYINFO_H
#define QUERYINFO_H

#include "svrdll.h"

#ifdef QUERYINFO_EXPORTS
#define QUERYINFO_API __declspec(dllexport)
#else
#define QUERYINFO_API __declspec(dllimport)
#endif

const dll_info* GetDllFuncs();

int GetDllFuncCnt();

int QueryInfomation(conn s, char* buf, int n, writeToClient fn);


#endif