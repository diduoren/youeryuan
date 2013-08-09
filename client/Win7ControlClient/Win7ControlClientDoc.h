// Win7ControlClientDoc.h : interface of the CWin7ControlClientDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIN7CONTROLCLIENTDOC_H__E879E2CE_B8F7_439F_BAA1_B20778E35681__INCLUDED_)
#define AFX_WIN7CONTROLCLIENTDOC_H__E879E2CE_B8F7_439F_BAA1_B20778E35681__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWin7ControlClientDoc : public PluginDoc
{
protected: // create from serialization only
	CWin7ControlClientDoc();
	DECLARE_DYNCREATE(CWin7ControlClientDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWin7ControlClientDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWin7ControlClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWin7ControlClientDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIN7CONTROLCLIENTDOC_H__E879E2CE_B8F7_439F_BAA1_B20778E35681__INCLUDED_)
