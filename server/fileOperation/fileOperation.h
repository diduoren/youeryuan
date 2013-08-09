#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H

#include "svrdll.h"

#ifdef FILEOPERATION_EXPORTS
#define FILEOPERATION_API __declspec(dllexport)
#else
#define FILEOPERATION_API __declspec(dllimport)
#endif

const dll_info* GetDllFuncs();

int GetDllFuncCnt();

int EnumAllDrives(conn s, char* buf, int n, writeToClient fn);

int EnumDir(conn s, char* buf, int n, writeToClient fn);

int CreateDir(conn s, char* buf, int n, writeToClient fn);

int CopyFileOrDir(conn s, char* buf, int n, writeToClient fn);

int RenameFileOrDir(conn s, char* buf, int n, writeToClient fn);

int DeleteFileOrDir(conn s, char* buf, int n, writeToClient fn);

int UploadFileOrDir(conn s, char* buf, int n, writeToClient fn);

int DownloadFileOrDir(conn s, char* buf, int n, writeToClient fn);

int SearchFile(conn s, char* buf, int n, writeToClient fn);

int ExecFile(conn s, char* buf, int n, writeToClient fn);

int OnInitialize(void *p);

#endif