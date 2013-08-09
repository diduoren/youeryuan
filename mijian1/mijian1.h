// mijian1.h : main header file for the MIJIAN1 application
//

#if !defined(AFX_MIJIAN1_H__EDC9C45B_23E8_4922_8814_031C7A7A81B3__INCLUDED_)
#define AFX_MIJIAN1_H__EDC9C45B_23E8_4922_8814_031C7A7A81B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMijian1App:
// See mijian1.cpp for the implementation of this class
//

class CMijian1App : public CWinApp
{
public:
	CMijian1App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMijian1App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMijian1App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIJIAN1_H__EDC9C45B_23E8_4922_8814_031C7A7A81B3__INCLUDED_)
