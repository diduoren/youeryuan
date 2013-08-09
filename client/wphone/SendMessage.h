#pragma once

#include "resource.h"
#include "afxwin.h"

class SendMessageForm : public CFormView
{
	DECLARE_DYNCREATE(SendMessageForm)

public:
	SendMessageForm();   // standard constructor
	virtual ~SendMessageForm();

// Dialog Data
	enum { IDD = IDD_SENDSMS };

	void SetReceiver(CString to);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSendMessage();
	afx_msg void OnClear();
	
private:
	CString m_to;
	CString m_body;
};
