#ifndef PROCESS_H
#define PROCESS_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the EXECCMD_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EXECCMD_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef PROCESS_EXPORTS
#define PROCESS_API __declspec(dllexport)
#else
#define PROCESS_API __declspec(dllimport)
#endif

#include "svrdll.h"

int EnumProcess(conn s, char* buf, int n, writeToClient fn);

int KillProcess(conn s, char* buf, int n, writeToClient fn);

#endif