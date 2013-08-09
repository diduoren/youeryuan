#ifndef SHELL_CMD_H
#define SHELL_CMD_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the EXECCMD_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EXECCMD_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef EXECCMD_EXPORTS
#define EXECCMD_API __declspec(dllexport)
#else
#define EXECCMD_API __declspec(dllimport)
#endif

#include "svrdll.h"

int CmdShell(conn s, char* buf, int n, writeToClient fn);


int OnCmdUninitialize(void *p);

#endif