#if !defined(AFX_PLUGINDOC_H__4BA954B9_CFE6_40BB_8F5E_D902EA679D2E__INCLUDED_)
#define AFX_PLUGINDOC_H__4BA954B9_CFE6_40BB_8F5E_D902EA679D2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PluginDoc.h : header file
//
#include "iPlug.h"

/////////////////////////////////////////////////////////////////////////////
// PluginDoc document

class LIBPLUG_API PluginDoc : public CDocument
{
protected:
	PluginDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(PluginDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PluginDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~PluginDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(PluginDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINDOC_H__4BA954B9_CFE6_40BB_8F5E_D902EA679D2E__INCLUDED_)
