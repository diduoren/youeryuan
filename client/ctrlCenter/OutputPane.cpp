#include "stdafx.h"
#include "OutputPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_OUTPANE_RBUTTONDOWN		10001
#define ID_LISTBOX					1001

/////////////////////////////////////////////////////////////////////////////
// COutputPane

COutputPane::COutputPane()
{
}

COutputPane::~COutputPane()
{
}


BEGIN_MESSAGE_MAP(COutputPane, CWnd)
	//{{AFX_MSG_MAP(COutputPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_LOG_DELETE_ONE, OnUpdateLogDeleteOne)
	ON_UPDATE_COMMAND_UI(ID_LOG_CLEAR_ALL, OnUpdateLogClearAll)
	ON_NOTIFY(WM_LIST_CLEAR,ID_LISTBOX,OnMessageClear)
	ON_COMMAND(ID_LOG_DELETE_ONE, OnLogDeleteOne)
	ON_COMMAND(ID_LOG_CLEAR_ALL, OnLogClearAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COutputPane message handlers

int COutputPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_StcCtrl.Create(_T(""), WS_CHILD|WS_VISIBLE, CRect(10,10,150,50), this );

	if (m_wndOutput.GetSafeHwnd() == 0)
	{
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP;
		if (!m_wndOutput.Create( dwStyle, CRect(0,0,0,0), this,	ID_LISTBOX ))
			return -1;

		m_Font.CreateFont( 12,                        // nHeight
							0,                         // nWidth
							0,                         // nEscapement
							0,                         // nOrientation
							FW_NORMAL,                 // nWeight
							FALSE,                     // bItalic
							FALSE,                     // bUnderline
							0,                         // cStrikeOut
							ANSI_CHARSET,              // nCharSet
							OUT_DEFAULT_PRECIS,        // nOutPrecision
							CLIP_DEFAULT_PRECIS,       // nClipPrecision
							DEFAULT_QUALITY,           // nQuality
							DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
							"宋体");                 // lpszFacename

		m_wndOutput.SetFont( &m_Font );
		
		
	}
	return 0;
}

void COutputPane::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);	

	m_StcCtrl.MoveWindow( 1, 1, cx-2, cy-1 );

	if (m_wndOutput.GetSafeHwnd())
	{
		m_wndOutput.MoveWindow(2, 2, cx-4, cy-3);
		m_wndOutput.Invalidate();
	}
	
}

void COutputPane::WriteLog(const char *cLog)
{
	m_wndOutput.AddString(CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S    ")+cLog);
	int nCount = 0;
	nCount = m_wndOutput.GetCount();
	if(nCount>0)
		m_wndOutput.SetCurSel(nCount-1);
}

void COutputPane::OnMessageClear(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	CPoint point;
	GetCursorPos(&point);
	
	CMenu menuPopup;
	menuPopup.CreatePopupMenu();
	menuPopup.AppendMenu(MF_STRING, ID_LOG_DELETE_ONE, "&删除选中的日志信息");
	menuPopup.AppendMenu(MF_STRING, ID_LOG_CLEAR_ALL, "&删除所有日志信息");
	
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	menuPopup.DestroyMenu();
}


void COutputPane::OnUpdateLogDeleteOne(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_wndOutput.GetCurSel()!=LB_ERR && m_wndOutput.GetCount()>0 )
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void COutputPane::OnUpdateLogClearAll(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_wndOutput.GetCount()>0 )
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void COutputPane::OnLogDeleteOne() 
{
	// TODO: Add your command handler code here
	int iIndex = m_wndOutput.GetCurSel();
	if( iIndex != LB_ERR )
	{
		m_wndOutput.DeleteString(iIndex);
	}
}

void COutputPane::OnLogClearAll() 
{
	// TODO: Add your command handler code here
	int iCount = m_wndOutput.GetCount();
	for ( int i=0; i<iCount; i++ )
	{
		m_wndOutput.DeleteString( 0 );
	}
}
