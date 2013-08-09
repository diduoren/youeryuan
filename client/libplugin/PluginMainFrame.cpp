// PluginMainFrame.cpp: implementation of the PluginMainFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PluginMainFrame.h"
#include "pluginMgr.h"
#include "MyProgress.h"
#include <list>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const char* mutexname = "GLOBAL_Jordan_Trimps";
static const char* visiblemsg = "SetVisible";

UINT WM_LIBPLUGIN_VISIBLE = RegisterWindowMessage(visiblemsg);

IMPLEMENT_DYNAMIC(PluginMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(PluginMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(PluginMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(WM_TRAY_EXIT, OnExitApp)
	ON_MESSAGE(ZCM_PROGRESS, OnProgressDlgNotify)
	ON_MESSAGE(WM_SYSTEMTRAY, OnSystemTray)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,          
	ID_SEPARATOR,        
	ID_SEPARATOR,
	
};

/////////////////////////////////////////////////////////////////////////////
// PluginMainFrame construction/destruction

PluginMainFrame::PluginMainFrame()
{
	// TODO: add member initialization code here
	
}

PluginMainFrame::~PluginMainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// PluginMainFrame diagnostics

#ifdef _DEBUG
void PluginMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void PluginMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

LRESULT PluginMainFrame::OnProgressDlgNotify(WPARAM wp, LPARAM lp)
{
	ISite::StructProgressNotify* progressStruct = (ISite::StructProgressNotify*)wp;
	MyProgress* progress = (MyProgress*)progressStruct->dlg;
	switch(progressStruct->id)
	{
	case ISite::CREATE_DLG:
		progress = new MyProgress((MyProgress::TRANSTYPE)progressStruct->data, this);
		progress->Create(IDD_PROGRESS, CWnd::GetDesktopWindow());
		progress->ShowWindow(SW_SHOW);
		progressStruct->dlg = progress;
		break;
	case ISite::INIT_SRC:
		progress->SetSource((const char*)progressStruct->data);
		break;
	case ISite::INIT_DST:
		progress->SetTarget((const char*)progressStruct->data);
		break;
	case ISite::SET_NAME:
		progress->SetFile((const char*)progressStruct->data);
		break;
	case ISite::SET_DIR:
		progress->SetSubdir((const char*)progressStruct->data);
		break;
	case ISite::SET_SIZE:
		progress->SetTotalBytes(*(__int64*)progressStruct->data);
		break;
	case ISite::SET_PROGRESS:
		progress->SetElapsedBytes(*(__int64*)progressStruct->data);
		break;
	case ISite::SET_SPEED:
		progress->SetSpeed(*(__int64*)progressStruct->data);
		break;
	case ISite::SET_TIME:
		progress->SetSpeed(*(__int64*)progressStruct->data, 1);
		break;
	case ISite::DESTROY_DLG:
		delete progress;
		break;
	default:
		break;
	}
	
	return TRUE;
}

CXTPMenuBar* PluginMainFrame::RegisterAllFrameMenu(LPCTSTR name,UINT UID)
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPMenuBar* pMenuBar = pCommandBars->SetMenu(name, UID);
	if(!pMenuBar)	return NULL;

	pMenuBar->SetFlags(xtpFlagIgnoreSetMenuMessage);
	m_defaultMenu.LoadMenu(UID);

	//遍历所有的插件，调用其相应的函数
	PluginMgr* mgr = PluginMgr::GetInstance();
	const std::list<DllWrapper*>& allDlls = mgr->m_dllList;
	for(std::list<DllWrapper*>::const_iterator it = allDlls.begin(); it != allDlls.end(); it++)
	{
		DllWrapper* wrapper = *it;
		IPlugProxy& proxy = wrapper->GetPlugIn();
		if (proxy)
		{
			proxy->OnCreateFrameMenu(&m_defaultMenu);
		}
	}

	pMenuBar->LoadMenu(&m_defaultMenu);
	
	return pMenuBar;
}

CXTPToolBar* PluginMainFrame::RegisterAllFrameToolBar(LPCTSTR name, UINT UID)
{	
	PluginMgr* mgr = PluginMgr::GetInstance();

	if(!mgr->GetCount())	return NULL;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPToolBar*	pCommandBar = (CXTPToolBar*)pCommandBars->Add(name, xtpBarTop);

	if (!pCommandBar)
	{
		TRACE0("Failed to create toolbar\n");
		return NULL;
	}
	if (UID && !pCommandBar->LoadToolBar(UID))
	{
		TRACE0("Failed to create toolbar\n");
		return NULL;
	}

	UINT allcount = 0;
	const std::list<DllWrapper*>& allDlls = mgr->m_dllList;
	for(std::list<DllWrapper*>::const_iterator it = allDlls.begin(); it != allDlls.end(); it++)
	{
		DllWrapper* wrapper = *it;
		IPlugProxy& proxy = wrapper->GetPlugIn();
		if (proxy)
		{
			UINT count = 0;
			ToolBarData	arr[WM_PLUG_EACH];
			proxy->OnCreateFrameToolBar(arr,count);
			count = count < WM_PLUG_EACH ? count : WM_PLUG_EACH - 1;

			for (UINT i = 0; i < count; i++)
			{
				ToolBarData data = arr[i];
				CXTPControl* pControl = pCommandBar->GetControls()->Add(xtpControlButton,data.uId);
				pControl->SetCaption(data.sName);
				pControl->SetDescription(data.sDesc);
				pControl->SetTooltip(data.sTip);
				pControl->SetIconId(allcount + i + 1);
				pControl->SetBeginGroup(data.bGroup);
			}
			allcount += count;
		}
	}

	return pCommandBar;
}

void PluginMainFrame::RegisterAllDockPane(CXTPDockingPaneManager* paneMgr,
				CXTPDockingPane* left,CXTPDockingPane* bottom,CXTPDockingPane* right)
{
	CXTPDockingPane* first_left = NULL;
	CXTPDockingPane* first_bottom = NULL;
	CXTPDockingPane* first_right = NULL;

	UINT allcount = 0;
	PluginMgr* mgr = PluginMgr::GetInstance();
	const std::list<DllWrapper*>& allDlls = mgr->m_dllList;
	for(std::list<DllWrapper*>::const_iterator it = allDlls.begin(); 
		it != allDlls.end(); it++)
	{
		DllWrapper* wrapper = *it;
		IPlugProxy& proxy = wrapper->GetPlugIn();
		if (proxy)
		{
			DockPane arr[WM_PLUG_EACH];
			UINT count = 0;
		
			proxy->OnCreateDockPane(arr,count);
			count = count < WM_PLUG_EACH ? count : WM_PLUG_EACH - 1;
			for (UINT j = 0; j < count; j++)
			{
				DockPane dp = arr[j];
				CXTPDockingPane* pane = paneMgr->CreatePane(dp.uId, dp.rect, (XTPDockingPaneDirection)dp.direct);
				switch (dp.direct)
				{
				case DockLeft:
					if (left)
						paneMgr->AttachPane(pane,left);
					left = pane;
					if (!first_left)
						first_left = left;
					break;
				case DockRight:
					if (left)
						paneMgr->AttachPane( pane,right);
					right = pane;
					if (!first_right)
						first_right = right;
					break;
				case DockBottom:
					if (bottom)
						paneMgr->AttachPane( pane,bottom);
					bottom = pane;
					if (!first_bottom)
						first_bottom = bottom;
					break;
				}
				pane->SetIconID(/*proxy.GetFirstIconId() + */dp.uIconId);
				pane->SetTitle(dp.sName);
				if (dp.pWnd)	
				{
					dp.pWnd->Create(_T("STATIC"), NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
						CRect(0,0,150,150), this, 0);
					pane->Attach(dp.pWnd);
				}
			}
			allcount += count;
		}
	}

	if (first_left)
		first_left->Select();
	if (first_right)
		first_right->Select();
	if (first_bottom)
		first_bottom->Select();
}

CImageList* PluginMainFrame::RegisterAllImage(int cx,int cy)
{
	if (!m_imgList.GetSafeHandle())
	{
		m_imgList.Create(cx, cy, ILC_MASK|ILC_COLOR24, 0, 1);
		
		CBitmap bmp;
		bmp.CreateBitmap(cx,cy,0,0,NULL);
		m_imgList.Add(&bmp,RGB(0,0,0));
	}

	PluginMgr* mgr = PluginMgr::GetInstance();
	if (!mgr->GetCount())	return NULL;

	const std::list<DllWrapper*>& allDlls = mgr->m_dllList;
	for(std::list<DllWrapper*>::const_iterator it = allDlls.begin(); 
		it != allDlls.end(); it++)
	{
		DllWrapper* wrapper = *it;
		IPlugProxy& proxy = wrapper->GetPlugIn();

		if(proxy)
		{
			ToolBarData arr[WM_PLUG_EACH];
			UINT count = 0;
			proxy->OnCreateFrameToolBar(arr, count);

			for (UINT i = 0; i < count; i++)
			{
				int startIconId = m_imgList.GetImageCount();
				proxy.SetFirstIconId(startIconId);
				int curImgCount = wrapper->GetImage(m_imgList, arr[i].uIconId);
			}
		}
	}
	
	return &m_imgList;
}

void PluginMainFrame::OnCommandUI(CCmdUI* pCmdUI)
{
	//if (m_paneMgr->IsPaneClosed(pCmdUI->m_nID))
	//	pCmdUI->SetCheck(FALSE);
	//else
	//	pCmdUI->SetCheck(TRUE);
}

int PluginMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//通过查询Mutex确定是否已经有一个实例在运行
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexname);
	if(mutex)
	{
		//向之前的窗口发送消息
		::SendNotifyMessage(HWND_BROADCAST, WM_LIBPLUGIN_VISIBLE, 0, TRUE);
		return -1;
	}
	else	
		mutex = CreateMutex(NULL, FALSE, mutexname);


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneText(1, "等待肉鸡用户连接...");

	//停靠栏
	m_nid.cbSize = sizeof(m_nid);
	m_nid.hWnd = m_hWnd;
	m_nid.uID = 0;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_SYSTEMTRAY;
	m_nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAIN);
	strcpy(m_nid.szTip, "监控客户端");
	Shell_NotifyIconA(NIM_ADD, &m_nid);

	return 0;
}

void PluginMainFrame::OnClose()
{
	//关闭窗口只是将其最小化
	ShowWindow(SW_HIDE);
}

void PluginMainFrame::OnDestroy()
{
	Shell_NotifyIconA(NIM_DELETE, &m_nid);
}

void PluginMainFrame::OnExitApp()
{
	//实际的关闭窗口
	if(CWnd::MessageBox("你确定关闭程序?", "网络远程监控Pro版", MB_ICONWARNING|MB_YESNO) == IDYES)
		DestroyWindow();
	else
		ShowWindow(SW_SHOW);
}

LRESULT PluginMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_LIBPLUGIN_VISIBLE)
	{
		//收到另一个实例发送的消息，将其显示
		ShowWindow(SW_RESTORE);
		SetForegroundWindow();
		return 0;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

LRESULT PluginMainFrame::OnSystemTray(WPARAM wParam, LPARAM lParam)
{
	int eventId = lParam;

	if(WM_LBUTTONUP == eventId)
	{
		//左键只是将其激活并显示
		ShowWindow(SW_SHOW);
	}
	else if(WM_RBUTTONUP == eventId)
	{
		//显示菜单选项
		CMenu menu;
		menu.CreatePopupMenu();
		
		menu.AppendMenu(MF_STRING, ID_APP_ABOUT, "关于");
		menu.AppendMenu(MF_STRING, WM_TRAY_EXIT, "退出");
		
		CPoint pt;
		GetCursorPos(&pt);
		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		menu.DestroyMenu();
	}

	return 0;
}