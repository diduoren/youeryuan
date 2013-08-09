// mijian1Dlg.h : header file
//

#if !defined(AFX_MIJIAN1DLG_H__888DE20D_4C13_4C8D_BEDB_3FCBD8A38FE1__INCLUDED_)
#define AFX_MIJIAN1DLG_H__888DE20D_4C13_4C8D_BEDB_3FCBD8A38FE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMijian1Dlg dialog

class CMijian1Dlg : public CDialog
{
// Construction
public:
	CMijian1Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMijian1Dlg)
	enum { IDD = IDD_MIJIAN1_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMijian1Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	int DoActualWork();

// Implementation
protected:
	HICON m_hIcon;
	unsigned int m_eventId;

	// Generated message map functions
	//{{AFX_MSG(CMijian1Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIJIAN1DLG_H__888DE20D_4C13_4C8D_BEDB_3FCBD8A38FE1__INCLUDED_)
