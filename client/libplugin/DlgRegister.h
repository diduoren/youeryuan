#if !defined(AFX_DLGREGISTER_H__F035BEB8_1D3C_46CB_991E_75C333F8D5AC__INCLUDED_)
#define AFX_DLGREGISTER_H__F035BEB8_1D3C_46CB_991E_75C333F8D5AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRegister.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRegister dialog

class CDlgRegister : public CDialog
{
// Construction
public:
	CDlgRegister(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRegister)
	enum { IDD = IDD_DIALOG_REGISTER };
	CString	m_strRegCode;
	CString	m_strUserID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRegister)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRegister)
	afx_msg void OnChangeEditRegCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREGISTER_H__F035BEB8_1D3C_46CB_991E_75C333F8D5AC__INCLUDED_)
