#if !defined(AFX_FORMCMD_H__A7E9E86E_4E52_4BD6_A427_69914C249DDD__INCLUDED_)
#define AFX_FORMCMD_H__A7E9E86E_4E52_4BD6_A427_69914C249DDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormCmd.h : header file
//

#include "Resource.h"
#include "ConsoleEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CFormCmd form view

class CFormCmd : public CFormView
{
	DECLARE_DYNCREATE(CFormCmd)
public:	
	CFormCmd(); 
	virtual ~CFormCmd();

	enum { IDD = IDD_CFORMCTRL };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormCmd)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void UpdateListDate(const char* GUID);
	
	void ClearListDate(const char* GUID);

	void AddCmdResult(int resp, const char* text, int len, bool end);
	
	static void UpdateCmdView(const char* GUID, CFormCmd* pView = NULL);

	static void FreeListData();
	
	void  CreateCmdThread();
	static unsigned int WINAPI SendCmdThread(void* param);

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Attributes
public:
	CConsoleEdit m_edit;
	static int staticLen ;
	static char m_buf[255];
	unsigned int m_eventId;

	// Generated message map functions
	//{{AFX_MSG(CFormCmd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnConsoleEnter(NMHDR *pNotifyStruct, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMCMD_H__A7E9E86E_4E52_4BD6_A427_69914C249DDD__INCLUDED_)
