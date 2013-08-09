// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Win7ControlClient.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, PluginMainFrame)

BEGIN_MESSAGE_MAP(CMainFrame, PluginMainFrame)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (PluginMainFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!InitCommandBars())
		return -1;

	CXTPPaintManager::SetTheme(xtpThemeWhidbey);

	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPMenuBar* pMenuBar = RegisterAllFrameMenu("菜单",IDR_MAINFRAME);
	
	CImageList* pimgList = RegisterAllImage(48, 48);
	CXTPToolBar* pPlugInBar = RegisterAllFrameToolBar("插件工具");
	
	if (pPlugInBar)
	{
		pPlugInBar->SetIconSize(CSize(48,48));
		if (pimgList)
			pPlugInBar->GetImageManager()->SetIcons(*pimgList,NULL,pimgList->GetImageCount(),0);
	}

	/*
	CXTPToolBar* pStandardBar = (CXTPToolBar*)pCommandBars->Add("标准", xtpBarTop);
	if (!pStandardBar || !pStandardBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (pPlugInBar)
		DockRightOf(pPlugInBar,pStandardBar);
	*/

	m_paneManager.InstallDockingPanes(this);
	m_paneManager.SetTheme(xtpPaneThemeWhidbey);
	RegisterAllDockPane(&m_paneManager);

	CXTPImageManager* pImageManager = pCommandBars->GetImageManager();
	pImageManager->InternalAddRef();
	
	m_paneManager.SetImageManager(pImageManager);
	m_paneManager.SetAlphaDockingContext(TRUE);
	m_paneManager.SetShowDockingContextStickers(TRUE);

	VERIFY(m_MTIClientWnd.Attach(this, TRUE));
	m_MTIClientWnd.EnableToolTips();
	m_MTIClientWnd.GetPaintManager()->SetAppearance(xtpTabAppearanceExcel);
	m_MTIClientWnd.GetPaintManager()->SetColor(xtpTabColorVisualStudio);
	m_MTIClientWnd.GetPaintManager()->SetOneNoteColors(TRUE);
	m_MTIClientWnd.GetPaintManager()->m_bShowIcons = FALSE;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

