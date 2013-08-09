#pragma once



// EmailForm form view

class EmailForm : public CFormView
{
	DECLARE_DYNCREATE(EmailForm)

protected:
	EmailForm();           // protected constructor used by dynamic creation
	virtual ~EmailForm();

public:
	enum { IDD = IDD_EMAI_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	afx_msg void OnClear();
	void SetReceiver(CString to);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSendMessage();

	DECLARE_MESSAGE_MAP()

private:
	CString m_to;
	CString m_body;
	CString m_sub;
};


