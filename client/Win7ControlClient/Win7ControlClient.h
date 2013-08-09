// Win7ControlClient.h : main header file for the WIN7CONTROLCLIENT application
//

#if !defined(AFX_WIN7CONTROLCLIENT_H__666C3ABB_7605_4D5D_865E_BB370D2F6FA5__INCLUDED_)
#define AFX_WIN7CONTROLCLIENT_H__666C3ABB_7605_4D5D_865E_BB370D2F6FA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientApp:
// See Win7ControlClient.cpp for the implementation of this class
//

class CWin7ControlClientApp : public PluginApp
{
public:
	CWin7ControlClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWin7ControlClientApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWin7ControlClientApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIN7CONTROLCLIENT_H__666C3ABB_7605_4D5D_865E_BB370D2F6FA5__INCLUDED_)
