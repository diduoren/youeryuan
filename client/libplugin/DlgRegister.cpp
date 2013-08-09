// DlgRegister.cpp : implementation file
//

#include "stdafx.h"
#include "DlgRegister.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRegister dialog


CDlgRegister::CDlgRegister(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRegister::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRegister)
	m_strRegCode = _T("");
	m_strUserID = _T("");
	//}}AFX_DATA_INIT
}


void CDlgRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRegister)
	DDX_Text(pDX, IDC_EDIT_REGCODE, m_strRegCode);
	DDX_Text(pDX, IDC_EDIT_USERID, m_strUserID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRegister, CDialog)
	//{{AFX_MSG_MAP(CDlgRegister)
	ON_EN_CHANGE(IDC_EDIT_REGCODE, OnChangeEditRegCode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRegister message handlers

void CDlgRegister::OnChangeEditRegCode() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData(true);
}
