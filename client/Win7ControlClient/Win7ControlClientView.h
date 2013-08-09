// Win7ControlClientView.h : interface of the CWin7ControlClientView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIN7CONTROLCLIENTVIEW_H__B0CC3A48_7BC0_4BD9_90DC_7A0F53C3BF2A__INCLUDED_)
#define AFX_WIN7CONTROLCLIENTVIEW_H__B0CC3A48_7BC0_4BD9_90DC_7A0F53C3BF2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWin7ControlClientView : public CView
{
protected: // create from serialization only
	CWin7ControlClientView();
	DECLARE_DYNCREATE(CWin7ControlClientView)

// Attributes
public:
	CWin7ControlClientDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWin7ControlClientView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWin7ControlClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWin7ControlClientView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Win7ControlClientView.cpp
inline CWin7ControlClientDoc* CWin7ControlClientView::GetDocument()
   { return (CWin7ControlClientDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIN7CONTROLCLIENTVIEW_H__B0CC3A48_7BC0_4BD9_90DC_7A0F53C3BF2A__INCLUDED_)
