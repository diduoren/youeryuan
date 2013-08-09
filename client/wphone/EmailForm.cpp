// EmailForm.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "EmailForm.h"
#include "WphonePlugin.h"

// EmailForm

IMPLEMENT_DYNCREATE(EmailForm, CFormView)

EmailForm::EmailForm()
	: CFormView(EmailForm::IDD)
{

}

EmailForm::~EmailForm()
{
}

void EmailForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_TO, m_to);	
	DDX_Text(pDX, IDC_EDIT_SUBJECT, m_sub);
	DDX_Text(pDX, IDC_EDIT_BODY, m_body);
}

BEGIN_MESSAGE_MAP(EmailForm, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SND, &EmailForm::OnSendMessage)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &EmailForm::OnClear)
END_MESSAGE_MAP()


// EmailForm diagnostics

#ifdef _DEBUG
void EmailForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void EmailForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void EmailForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	CRect rect0, rect1, rect2, rect3;
	CWnd* pWnd;
	int W = rect.Width(), H = rect.Height();

	//重新排列各个控件的相对关系

	pWnd = this->GetDlgItem(IDC_BUTTON_SND);
	if( pWnd != NULL)
		pWnd->MoveWindow(20, 20, 48, 30);		

	pWnd = this->GetDlgItem(IDC_BUTTON_CLEAR);
	if( pWnd != NULL)
		pWnd->MoveWindow(W-20-48, 20, 48, 30);	

	pWnd = this->GetDlgItem(IDC_STATIC1);
	if( pWnd != NULL)
		pWnd->MoveWindow(20, 60, 48, 30);	

	pWnd = this->GetDlgItem(IDC_EDIT_TO);
	if( pWnd != NULL)
		pWnd->MoveWindow(100, 60, W-120, 30);		

	pWnd = this->GetDlgItem(IDC_STATIC2);
	if( pWnd != NULL)
		pWnd->MoveWindow(20, 100, 48, 30);	

	pWnd = this->GetDlgItem(IDC_EDIT_SUBJECT);
	if( pWnd != NULL)
		pWnd->MoveWindow(100, 100, W-120, 30);

	pWnd = this->GetDlgItem(IDC_EDIT_BODY);
	if( pWnd != NULL)
		pWnd->MoveWindow(20, 140,  W-40, H-160);

}

void EmailForm::OnSendMessage()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if(m_to == "" || m_body == "" || m_sub == "")
	{
		AfxMessageBox("接收者、主题、邮件内容都不能为空!", MB_ICONERROR);
		return;
	}

	WPhonePlugin::SendEmail(m_to, m_sub, m_body);
}

void EmailForm::OnClear()
{
	m_to = m_body = m_sub = "";
	
	UpdateData(FALSE);
}

void EmailForm::SetReceiver(CString to)
{
	m_to = to;
	
	UpdateData(FALSE);
}