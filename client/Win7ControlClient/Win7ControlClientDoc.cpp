// Win7ControlClientDoc.cpp : implementation of the CWin7ControlClientDoc class
//

#include "stdafx.h"
#include "Win7ControlClient.h"

#include "Win7ControlClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientDoc

IMPLEMENT_DYNCREATE(CWin7ControlClientDoc, PluginDoc)

BEGIN_MESSAGE_MAP(CWin7ControlClientDoc, PluginDoc)
	//{{AFX_MSG_MAP(CWin7ControlClientDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientDoc construction/destruction

CWin7ControlClientDoc::CWin7ControlClientDoc()
{
	// TODO: add one-time construction code here

}

CWin7ControlClientDoc::~CWin7ControlClientDoc()
{
}

BOOL CWin7ControlClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientDoc serialization

void CWin7ControlClientDoc::Serialize(CArchive& ar)
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
// CWin7ControlClientDoc diagnostics

#ifdef _DEBUG
void CWin7ControlClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWin7ControlClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWin7ControlClientDoc commands
