#pragma once


#include "resource.h"
// AudioForm form view

class AudioForm : public CFormView
{
	DECLARE_DYNCREATE(AudioForm)

protected:
	AudioForm();           // protected constructor used by dynamic creation
	virtual ~AudioForm();

public:
	enum { IDD = IDD_FORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	void OnOperationFailed(unsigned short cmd, unsigned short resp);
	void ProcessData(int len);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	bool hasVoice;
};


