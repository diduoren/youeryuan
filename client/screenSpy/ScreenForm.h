#pragma once

#include <afxcview.h>
#include "afxcmn.h"
#include "resource.h"

// ScreenForm form view

class ScreenForm : public CFormView
{
	DECLARE_DYNCREATE(ScreenForm)

protected:
	ScreenForm();           // protected constructor used by dynamic creation
	virtual ~ScreenForm();

public:
	enum { IDD = IDD_FORMVIEW };
	
	//处理响应数据
	void ProcessData(WPARAM wp);

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);


	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	//鼠标右键选中某个item
	afx_msg void OnRclickItem(NMHDR* pNMHDR, LRESULT* pResult); 

private:
	HICON m_hIcon;
	MINMAXINFO m_MMI;
	HDC m_hDC, m_hMemDC, m_hPaintDC;
	HBITMAP	m_hFullBitmap;
	LPVOID m_lpScreenDIB;
	LPBITMAPINFO m_FirstBMP, m_NextBMP;
	UINT m_nCount;
	UINT m_HScrollPos, m_VScrollPos;
	HCURSOR	m_hRemoteCursor;
	DWORD	m_dwCursor_xHotspot, m_dwCursor_yHotspot;
	POINT	m_RemoteCursorPos;
	BYTE	m_bCursorIndex;
	//CCursorInfo	m_CursorInfo;
	int	m_nBitCount;
	bool m_bIsFirst;
	bool m_bIsTraceCursor;
	bool m_bIsCtrl;
	bool m_Init;
	int m_NewWidth, m_NewHeight;
	int m_ImgSize;

public:
	void SendCommand(MSG* pMsg);
	void InitMMI();
	void DrawTipString(CString str);
	void ResetScreen(void* data);
	void DrawFirstScreen(void* data);
	void DrawNextScreenDiff();	// 差异法
	void DrawNextScreenRect(void* data);	// 隔行扫描法
	void SendResetScreen(int nBitCount);
	void SendResetAlgorithm(UINT nAlgorithm);
	bool SaveSnapshot();
	void OnOperationFailed(unsigned short cmd, unsigned short resp);
	LRESULT OnGetMiniMaxInfo(WPARAM, LPARAM);
	
private:

	DECLARE_MESSAGE_MAP()
};


