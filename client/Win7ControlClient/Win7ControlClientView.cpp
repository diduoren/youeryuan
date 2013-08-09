// Win7ControlClientView.cpp : implementation of the CWin7ControlClientView class
//

#include "stdafx.h"
#include "Win7ControlClient.h"

#include "Win7ControlClientDoc.h"
#include "Win7ControlClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientView

IMPLEMENT_DYNCREATE(CWin7ControlClientView, CView)

BEGIN_MESSAGE_MAP(CWin7ControlClientView, CView)
	//{{AFX_MSG_MAP(CWin7ControlClientView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientView construction/destruction

CWin7ControlClientView::CWin7ControlClientView()
{
	// TODO: add construction code here

}

CWin7ControlClientView::~CWin7ControlClientView()
{
}

BOOL CWin7ControlClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientView drawing

void CWin7ControlClientView::OnDraw(CDC* pDC)
{
	CWin7ControlClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientView printing

BOOL CWin7ControlClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWin7ControlClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWin7ControlClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientView diagnostics

#ifdef _DEBUG
void CWin7ControlClientView::AssertValid() const
{
	CView::AssertValid();
}

void CWin7ControlClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWin7ControlClientDoc* CWin7ControlClientView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWin7ControlClientDoc)));
	return (CWin7ControlClientDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientView message handlers
