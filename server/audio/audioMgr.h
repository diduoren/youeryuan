#ifndef AUDIOMGR_H
#define AUDIOMGR_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the EXECCMD_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EXECCMD_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef AHDIO_EXPORTS
#define AHDIO_API __declspec(dllexport)
#else
#define AHDIO_API __declspec(dllimport)
#endif

#include "svrdll.h"

int AudioMonitor(conn s, char* buf, int n, writeToClient fn);

int AudioDestroy(conn s, char* buf, int n, writeToClient fn);

const dll_info* GetDllFuncs();

int GetDllFuncCnt();

int OnInitialize(void *p);

#endif