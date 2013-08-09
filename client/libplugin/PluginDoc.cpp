// PluginDoc.cpp : implementation file
//

#include "stdafx.h"
#include "PluginDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PluginDoc

IMPLEMENT_DYNCREATE(PluginDoc, CDocument)

PluginDoc::PluginDoc()
{
}

BOOL PluginDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

PluginDoc::~PluginDoc()
{
}


BEGIN_MESSAGE_MAP(PluginDoc, CDocument)
	//{{AFX_MSG_MAP(PluginDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PluginDoc diagnostics

#ifdef _DEBUG
void PluginDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void PluginDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// PluginDoc serialization

void PluginDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// PluginDoc commands
