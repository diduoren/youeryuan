#pragma once

#include "resource.h"

// CCreateInjectServer dialog

class CCreateInjectServer : public CDialog
{
	DECLARE_DYNAMIC(CCreateInjectServer)

public:
	CCreateInjectServer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCreateInjectServer();

// Dialog Data
	enum { IDD = IDD_DIALOG_CREATE_CLIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	afx_msg void OnButtonCreate();
	afx_msg void OnBnClickedRadioVip();
	afx_msg void OnBnClickedRadioNovip();

private:
	CComboBox m_IpList;
	CComboBox m_InjectList;
	CComboBox m_ConnList;

	CString	m_Port;
	CString	m_URL;
	BOOL	m_IsVip;

	DECLARE_MESSAGE_MAP()
};
