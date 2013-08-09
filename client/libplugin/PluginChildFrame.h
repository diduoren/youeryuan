#if !defined(AFX_PLUGINCHILDFRAME_H__8D2A0C16_4E59_487E_A691_CC5FD095F282__INCLUDED_)
#define AFX_PLUGINCHILDFRAME_H__8D2A0C16_4E59_487E_A691_CC5FD095F282__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PluginChildFrame.h : header file
//
#include "iPlug.h"

/////////////////////////////////////////////////////////////////////////////
// PluginChildFrame frame

class LIBPLUG_API PluginChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(PluginChildFrame)
protected:
	PluginChildFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PluginChildFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~PluginChildFrame();

	// Generated message map functions
	//{{AFX_MSG(PluginChildFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINCHILDFRAME_H__8D2A0C16_4E59_487E_A691_CC5FD095F282__INCLUDED_)
