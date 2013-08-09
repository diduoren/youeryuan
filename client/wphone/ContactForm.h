#pragma once

#include "TreeListCtrl.h"
#include "resource.h"

class ContactForm : public CFormView
{
	DECLARE_DYNCREATE(ContactForm)

public:
	enum { IDD = IDD_FORMVIEW };
	ContactForm();
	~ContactForm();

	void ClearAll();
	
	void Insert(unsigned short seq, void* data);

private:
	void SortTree(int nCol, BOOL bAscending, HTREEITEM hParent);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnMenuExport();
	afx_msg void OnCallContact();
	afx_msg void OnSMSContact();
	afx_msg void OnEmailContact();

	DECLARE_MESSAGE_MAP()

private:
	CTreeListCtrl m_treeList;
	BOOL		m_init;
};
