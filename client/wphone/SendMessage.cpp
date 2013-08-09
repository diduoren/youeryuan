// SendMessage.cpp : implementation file
//

#include "stdafx.h"
#include "SendMessage.h"
#include "WPhonePlugin.h"

IMPLEMENT_DYNCREATE(SendMessageForm, CFormView)

SendMessageForm::SendMessageForm()
	: CFormView(SendMessageForm::IDD)
	, m_to(_T(""))
	, m_body(_T(""))
{

}

SendMessageForm::~SendMessageForm()
{
}

void SendMessageForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_TO, m_to);
	DDV_MaxChars(pDX, m_to, 16);

	DDX_Text(pDX, IDC_EDIT_BODY, m_body);
	DDV_MaxChars(pDX, m_body, 140);
}


BEGIN_MESSAGE_MAP(SendMessageForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_SND, &SendMessageForm::OnSendMessage)
END_MESSAGE_MAP()

void SendMessageForm::OnSendMessage()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if(m_to == "" || m_body == "")
	{
		AfxMessageBox("接收者或者信息内容不能为空!", MB_ICONERROR);
		return;
	}

	if(m_body.GetLength() > 140)
	{
		AfxMessageBox("信息内容长度小于140字!", MB_ICONERROR);
		return;
	}

	WPhonePlugin::SendMessage(m_to, m_body);
}

void SendMessageForm::OnClear()
{
	m_to = m_body = "";

	UpdateData(FALSE);
}

void SendMessageForm::SetReceiver(CString to)
{
	m_to = to;
	
	UpdateData(FALSE);
}